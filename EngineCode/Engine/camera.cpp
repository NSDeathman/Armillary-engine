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
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
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

	m_moveDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Calculate aspect ratio initially
	m_aspectRatio = static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight);
}

void CCamera::OnFrame()
{
	m_fov = D3DXToRadian(g_Fov);
	m_nearPlane = g_NearPlane;
	m_farPlane = g_FarPlane;

	// Update aspect ratio every frame (if resolution can change while running)
	m_aspectRatio = static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight);

	UpdateInput();
}

void CCamera::Reset()
{
	// Correctly reset the aspect ratio without adding to it
	m_aspectRatio = static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight);
}

// Calculate view matrix
D3DXMATRIX CCamera::GetViewMatrix()
{
	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &m_position, &m_direction, &m_upVec);
	return viewMatrix;
}

// Calculate projection matrix
D3DXMATRIX CCamera::GetProjectionMatrix()
{
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fov, m_aspectRatio, m_nearPlane, m_farPlane);

	if (g_UseOrthogonalProjection)
	{
		// Orthographic projection matrix
		float left = -g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float right = g_OrthogonalProjectionSize * m_aspectRatio * 0.5f;
		float bottom = -g_OrthogonalProjectionSize * 0.5f;
		float top = g_OrthogonalProjectionSize * 0.5f;

		D3DXMatrixOrthoLH(&projectionMatrix, right - left, top - bottom, m_nearPlane, m_farPlane);
	}
	else
	{
		// Perspective projection matrix
		D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	return projectionMatrix;
}

void CCamera::UpdateInput()
{
	float MoveAmount = 0.5f;
	float MoveSpeed = 1.0f;

	// Speed Up/Down
	if (Input->KeyHolded(SDL_SCANCODE_LCTRL))
		MoveSpeed *= 2.0f;
	else if (Input->KeyHolded(SDL_SCANCODE_LSHIFT))
		MoveSpeed /= 4.0f;

	// Move Forward/Backward
	if (Input->KeyHolded(SDL_SCANCODE_W))
		m_moveDirection.z += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_S))
		m_moveDirection.z -= MoveAmount;

	// Move Left/Right
	if (Input->KeyHolded(SDL_SCANCODE_D))
		m_moveDirection.x += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_A))
		m_moveDirection.x -= MoveAmount;

	// Move Up/Down
	if (Input->KeyHolded(SDL_SCANCODE_E))
		m_moveDirection.y += MoveAmount;
	else if (Input->KeyHolded(SDL_SCANCODE_Q))
		m_moveDirection.y -= MoveAmount;

	Move(m_moveDirection, MoveSpeed);

	m_moveDirection = D3DXVECTOR3(0, 0, 0);
}

// Update camera position
void CCamera::Move(const D3DXVECTOR3& direction, float amount)
{
	// Simple euler method to calculate position delta
	D3DXVECTOR3 vPosDelta = direction * amount;

	// If rotating the camera
	// if ((m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown)// || m_vGamePadRightThumb.x != 0
	// || m_vGamePadRightThumb.z != 0) if ((m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown)//
	// || m_vGamePadRightThumb.x != 0 || m_vGamePadRightThumb.z != 0)
	//{
	// Update the pitch & yaw angle based on mouse movement
	float fYawDelta = 0.0f; // m_vRotVelocity.x;
	float fPitchDelta = 0.0f; // m_vRotVelocity.y;

	// Invert pitch if requested
	//if (m_bInvertPitch)
	//	fPitchDelta = -fPitchDelta;

	float m_fCameraPitchAngle = 0.0f; //+= fPitchDelta;
	float m_fCameraYawAngle = 0.0f; //+= fYawDelta;

	// Limit pitch to straight up or straight down
	m_fCameraPitchAngle = __max(-D3DX_PI / 2.0f, m_fCameraPitchAngle);
	m_fCameraPitchAngle = __min(+D3DX_PI / 2.0f, m_fCameraPitchAngle);
	//}

	// Make a rotation matrix based on the camera's yaw & pitch
	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0);

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp, vWorldAhead;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &mCameraRot);
	D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &mCameraRot);

	// Transform the position delta by the camera's rotation
	D3DXVECTOR3 vPosDeltaWorld;
	//if (!m_bEnableYAxisMovement)
	//{
		// If restricting Y movement, do not include pitch
		// when transforming position delta vector.
	//	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_fCameraYawAngle, 0.0f, 0.0f);
	//}
	D3DXVec3TransformCoord(&vPosDeltaWorld, &vPosDelta, &mCameraRot);

	// Move the eye position
	m_position += vPosDeltaWorld;
	//if (m_bClipToBoundary)
	//	ConstrainToBoundary(&m_vEye);

	// Update the lookAt position based on the eye position
	m_direction = m_position + vWorldAhead;

	// Update the view matrix
	//D3DXMatrixLookAtLH(&m_mView, &m_vEye, &m_vLookAt, &vWorldUp);

	//D3DXMatrixInverse(&m_mCameraWorld, NULL, &m_mView);
}

// Rotate camera around the target point (simple placeholder)
void rotate(float yaw, float pitch)
{
	// Implementation for yaw/pitch adjustments would go here.
}
///////////////////////////////////////////////////////////////
