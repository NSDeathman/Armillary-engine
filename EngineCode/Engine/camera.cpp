///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#include "camera.h"
#include "log.h"
#include "main_window.h"
#include "Input.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
bool g_UseOrthogonalProjection = false;
float g_OrthogonalProjectionSize = 3.0f;
float g_Fov = 90.0f;
float g_Aspect = float(g_ScreenWidth) / float(g_ScreenHeight);
float g_NearPlane = 0.01f;
float g_FarPlane = 100.0f;
///////////////////////////////////////////////////////////////
void CCamera::Initialize()
{
	m_position = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_direction = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_upVec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_fov = D3DXToRadian(g_Fov);
	m_nearPlane = g_NearPlane;
	m_farPlane = g_FarPlane;
	m_aspectRatio = float(g_ScreenWidth) / float(g_ScreenHeight);
}

void CCamera::OnFrame()
{
	if (Input->NeedUpdateInput())
		UpdateInput();
}

void CCamera::Reset()
{
	m_aspectRatio = float(g_ScreenWidth) / float(g_ScreenHeight);
}

D3DXMATRIX CCamera::GetViewMatrix()
{
	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &m_position, &m_direction, &m_upVec);
	return viewMatrix;
}

D3DXMATRIX CCamera::GetProjectionMatrix()
{
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fov, m_aspectRatio, m_nearPlane, m_farPlane);

	if (g_UseOrthogonalProjection)
	{
		float left = -g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float right = g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float bottom = -g_OrthogonalProjectionSize * 0.5f;
		float top = g_OrthogonalProjectionSize * 0.5f;

		D3DXMatrixOrthoLH(&projectionMatrix, right - left, top - bottom, m_nearPlane, m_farPlane);
	}
	else
	{
		D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	return projectionMatrix;
}

void CCamera::UpdateInput()
{
	//---------Moving---------\\

	D3DXVECTOR3 MoveDirection = D3DXVECTOR3(0, 0, 0);
	float MoveAmount = 0.5f;
	float MoveSpeed = 1.0f;

	// Speed Up/Down
	if (Input->KeyHolded(SDL_SCANCODE_LCTRL))
		MoveSpeed *= 2.0f;
	else if (Input->KeyHolded(SDL_SCANCODE_LSHIFT))
		MoveSpeed /= 4.0f;

	// Move Forward/Backward
	if (Input->KeyHolded(SDL_SCANCODE_W) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_DPAD_UP))
		MoveDirection.z += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_S) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		MoveDirection.z -= MoveAmount;

	// Move Left/Right
	if (Input->KeyHolded(SDL_SCANCODE_D) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		MoveDirection.x += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_A) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		MoveDirection.x -= MoveAmount;

	// Move Up/Down
	if (Input->KeyHolded(SDL_SCANCODE_E))
		MoveDirection.y += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_Q))
		MoveDirection.y -= MoveAmount;

	//---------Rotating---------\\

	float RotateAmount = 0.1f;
	float YawDelta = 0.0f;
	float PitchDelta = 0.0f;

	// Rotate Up/Down
	if (Input->KeyHolded(SDL_SCANCODE_UP) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_Y))
		PitchDelta -= RotateAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_DOWN) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_A))
		PitchDelta += RotateAmount;

	// Rotate Right/Left
	if (Input->KeyHolded(SDL_SCANCODE_RIGHT) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_B))
		YawDelta += RotateAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_LEFT) || Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_X))
		YawDelta -= RotateAmount;

	//---------Applying---------\\
	// Send data to movement code
	Apply(MoveDirection, MoveSpeed, YawDelta, PitchDelta);

	//---------Clearing---------\\
	// Set direction and rotation zero value
	MoveDirection = D3DXVECTOR3(0, 0, 0);
	YawDelta = 0.0f;
	PitchDelta = 0.0f;
}

void CCamera::Apply(D3DXVECTOR3 direction, float amount, float yawdelta, float pitchdelta)
{
	// Simple euler method to calculate position delta
	D3DXVECTOR3 vPosDelta = direction * amount;

	m_yaw += yawdelta;
	m_pitch += pitchdelta;

	if (m_yaw = 360)
		m_yaw = 0.0f;

	if (m_yaw = 360)
		m_yaw = 0.0f;

	// Make a rotation matrix based on the camera's yaw & pitch
	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_yaw, m_pitch, 0);

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp, vWorldAhead;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &mCameraRot);
	D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &mCameraRot);

	// Transform the position delta by the camera's rotation
	D3DXVECTOR3 vPosDeltaWorld = D3DXVECTOR3(0, 0, 0);

	D3DXVec3TransformCoord(&vPosDeltaWorld, &vPosDelta, &mCameraRot);

	// Move the eye position
	m_position += vPosDeltaWorld;

	// Update the lookAt position based on the eye position
	m_direction = m_position + vWorldAhead;
}
///////////////////////////////////////////////////////////////
