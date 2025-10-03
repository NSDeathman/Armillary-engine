///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "camera.h"
#include "log.h"
#include "window_implementation.h"
#include "Input.h"
#include "Application_Deprecated.h"
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
bool g_bNeedLockCursor = false;
bool g_bNeedUpdateCameraInput = false;
bool g_UseOrthogonalProjection = false;
float g_OrthogonalProjectionSize = 3.0f;
float g_Fov = 90.0f;
float g_Aspect = float(g_ScreenWidth) / float(g_ScreenHeight);
float g_NearPlane = 0.01f;
float g_FarPlane = 100.0f;
///////////////////////////////////////////////////////////////
void CCamera::Initialize()
{
	SetDefaultParams();

	m_fov = D3DXToRadian(g_Fov);
	m_nearPlane = g_NearPlane;
	m_farPlane = g_FarPlane;
	m_aspectRatio = float(g_ScreenWidth) / float(g_ScreenHeight);

	GetCursorPos(&m_ptLastMousePosition);

	// Initialize smoothing targets
	m_targetPosition = m_position;
	m_targetYaw = m_yaw;
	m_targetPitch = m_pitch;
	m_currentVelocity = D3DXVECTOR3(0, 0, 0);
	m_yawVelocity = 0.0f;
	m_pitchVelocity = 0.0f;

	// Default smoothing settings
	SetSmoothing(0.1f, 0.05f, 50.0f);

	CalculateMatrices();
}

void CCamera::SetDefaultParams()
{
	m_position = D3DXVECTOR3(0.0f, 15.0f, -30.0f);
	m_direction = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
	m_upVec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_yaw = 0.0f;
	m_pitch = 0.0f;
}

void CCamera::OnFrame()
{
	float deltaTime = Application->GetTimeDelta();

	if (g_bNeedUpdateCameraInput)
	{
		UpdateInput();
	}
	else
	{
		GetCursorPos(&m_ptLastMousePosition);
	}

	// Apply smoothing every frame
	ApplySmoothing(deltaTime);
	CalculateMatrices();
}

void CCamera::Reset()
{
	m_aspectRatio = float(g_ScreenWidth) / float(g_ScreenHeight);
}

void CCamera::UpdateInput()
{
	float TimeDelta = Application->GetTimeDelta();

	//---------Moving---------\\

	D3DXVECTOR3 MoveDirection = D3DXVECTOR3(0, 0, 0);
	float MoveAmount = 0.5f + TimeDelta;
	float MoveSpeed = 1.0f;

	// Speed Up/Down
	if (KEY_HELD(SDL_SCANCODE_LCTRL))
		MoveSpeed *= 2.0f;
	else if (KEY_HELD(SDL_SCANCODE_LSHIFT))
		MoveSpeed /= 4.0f;

	// Move Forward/Backward
	if (KEY_HELD(SDL_SCANCODE_W))
		MoveDirection.z += MoveAmount;
	else if (KEY_HELD(SDL_SCANCODE_S))
		MoveDirection.z -= MoveAmount;

	// Move Left/Right
	if (KEY_HELD(SDL_SCANCODE_D))
		MoveDirection.x += MoveAmount;
	else if (KEY_HELD(SDL_SCANCODE_A))
		MoveDirection.x -= MoveAmount;

	// Move Up/Down
	if (KEY_HELD(SDL_SCANCODE_E) || GAMEPAD_BUTTON_HELD(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
		MoveDirection.y += MoveAmount;
	else if (KEY_HELD(SDL_SCANCODE_Q) || GAMEPAD_BUTTON_HELD(SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
		MoveDirection.y -= MoveAmount;

	float LeftStickX = 0.0f;
	float LeftStickY = 0.0f;
	INPUT.GetLeftStick(LeftStickX, LeftStickY);

	MoveDirection.x += LeftStickX * MoveAmount;
	MoveDirection.z -= LeftStickY * MoveAmount;

	//---------Rotating---------\\

	POINT ptCurMouseDelta;
	POINT ptCurMousePos;

	// Get current position of mouse
	GetCursorPos(&ptCurMousePos);

	// Calc how far it's moved since last frame
	ptCurMouseDelta.x = ptCurMousePos.x - m_ptLastMousePosition.x;
	ptCurMouseDelta.y = ptCurMousePos.y - m_ptLastMousePosition.y;

	// Record current position for next time
	m_ptLastMousePosition = ptCurMousePos;

	if (g_bNeedLockCursor)
	{
		// Set position of camera to center of desktop,
		// so it always has room to move.  This is very useful
		// if the cursor is hidden.  If this isn't done and cursor is hidden,
		// then invisible cursor will hit the edge of the screen
		// and the user can't tell what happened
		POINT ptCenter;

		// Get the center of the current monitor
		SDL_DisplayMode DisplayMode = DummyWindow->GetSDLDisplayMode();

		// Calculate center position
		ptCenter.x = DisplayMode.w / 2;
		ptCenter.y = DisplayMode.h / 2;

		SetCursorPos(ptCenter.x, ptCenter.y);

		m_ptLastMousePosition = ptCenter;
	}

	float MouseSens = 0.01f;

	float RotateAmount = 0.1f;
	float YawDelta = 0.0f;
	float PitchDelta = 0.0f;

	YawDelta += ptCurMouseDelta.x * MouseSens;
	PitchDelta += ptCurMouseDelta.y * MouseSens;

	// Rotate Up/Down
	if (KEY_HELD(SDL_SCANCODE_UP))
		PitchDelta -= RotateAmount;
	else if (KEY_HELD(SDL_SCANCODE_DOWN))
		PitchDelta += RotateAmount;

	// Rotate Right/Left
	if (KEY_HELD(SDL_SCANCODE_RIGHT))
		YawDelta += RotateAmount;
	else if (KEY_HELD(SDL_SCANCODE_LEFT))
		YawDelta -= RotateAmount;

	float RightStickX = 0.0f;
	float RightStickY = 0.0f;
	INPUT.GetRightStick(RightStickX, RightStickY);

	YawDelta += RightStickX * RotateAmount;
	PitchDelta += RightStickY * RotateAmount;

	//---------Applying---------\\
	// Send data to movement code
	ApplyMovement(MoveDirection, MoveSpeed, YawDelta, PitchDelta);

	//---------Clearing---------\\
	// Set direction and rotation zero value
	MoveDirection = D3DXVECTOR3(0, 0, 0);
	YawDelta = 0.0f;
	PitchDelta = 0.0f;
}

void CCamera::ApplyMovement(D3DXVECTOR3 direction, float amount, float yawdelta, float pitchdelta)
{
	// Update target rotation
	m_targetYaw += yawdelta;
	m_targetPitch += pitchdelta;

	m_targetPitch = std::min(m_targetPitch, 1.5f);
	m_targetPitch = std::max(m_targetPitch, -1.5f);

	// Make a rotation matrix based on the target yaw & pitch
	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_targetYaw, m_targetPitch, 0);

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp, vWorldAhead;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &mCameraRot);
	D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &mCameraRot);

	// Transform the position delta by the camera's rotation
	D3DXVECTOR3 vPosDeltaWorld = D3DXVECTOR3(0, 0, 0);

	// Simple euler method to calculate position delta
	D3DXVECTOR3 vPosDelta = direction * amount;

	D3DXVec3TransformCoord(&vPosDeltaWorld, &vPosDelta, &mCameraRot);

	// Update target position
	m_targetPosition += vPosDeltaWorld;

	// Update the lookAt position based on the target position
	m_direction = m_targetPosition + vWorldAhead;
}

void CCamera::ApplySmoothing(float deltaTime)
{
	// SmoothDamp for position
	D3DXVECTOR3 smoothPosition = m_position;

	// Calculate smooth position using exponential decay
	if (deltaTime > 0.0f)
	{
		D3DXVECTOR3 positionError = m_targetPosition - m_position;
		D3DXVECTOR3 acceleration = positionError / (m_positionSmoothTime * m_positionSmoothTime);

		m_currentVelocity += acceleration * deltaTime;

		// Apply damping
		float damping = 2.0f * sqrtf(1.0f / (m_positionSmoothTime * m_positionSmoothTime));
		m_currentVelocity -= m_currentVelocity * damping * deltaTime;

		// Limit maximum speed
		float currentSpeed = D3DXVec3Length(&m_currentVelocity);
		if (currentSpeed > m_maxSpeed)
		{
			m_currentVelocity = m_currentVelocity * (m_maxSpeed / currentSpeed);
		}

		smoothPosition += m_currentVelocity * deltaTime;
	}
	else
	{
		smoothPosition = m_targetPosition;
	}

	// SmoothDamp for rotation (simplified version)
	float smoothYaw = m_yaw;
	float smoothPitch = m_pitch;

	if (deltaTime > 0.0f)
	{
		// Exponential smoothing for rotation
		float yawAlpha = 1.0f - expf(-deltaTime / m_rotationSmoothTime);
		float pitchAlpha = 1.0f - expf(-deltaTime / m_rotationSmoothTime);

		smoothYaw = m_yaw + (m_targetYaw - m_yaw) * yawAlpha;
		smoothPitch = m_pitch + (m_targetPitch - m_pitch) * pitchAlpha;
	}
	else
	{
		smoothYaw = m_targetYaw;
		smoothPitch = m_targetPitch;
	}

	// Apply smoothed values
	m_position = smoothPosition;
	m_yaw = smoothYaw;
	m_pitch = smoothPitch;

	// Update direction vector with smoothed rotation
	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_yaw, m_pitch, 0);

	D3DXVECTOR3 vWorldAhead;
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &mCameraRot);

	m_direction = m_position + vWorldAhead;
}

void CCamera::CalculateMatrices()
{
	D3DXMatrixLookAtLH(&m_View, &m_position, &m_direction, &m_upVec);

	if (g_UseOrthogonalProjection)
	{
		float left = -g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float right = g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float bottom = -g_OrthogonalProjectionSize * 0.5f;
		float top = g_OrthogonalProjectionSize * 0.5f;

		D3DXMatrixOrthoLH(&m_Projection, right - left, top - bottom, m_nearPlane, m_farPlane);
	}
	else
	{
		D3DXMatrixPerspectiveFovLH(&m_Projection, m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
	}
}
///////////////////////////////////////////////////////////////
CCamera* Camera = nullptr;
///////////////////////////////////////////////////////////////
