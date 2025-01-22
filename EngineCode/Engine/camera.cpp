///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#include "camera.h"
#include "log.h"
#include "main_window.h"
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

// Update camera position
void CCamera::Move(const D3DXVECTOR3& direction, float amount)
{
	m_position += direction * amount;
	m_direction += direction * amount; // Keep the target relative to the camera position
}

// Rotate camera around the target point (simple placeholder)
void rotate(float yaw, float pitch)
{
	// Implementation for yaw/pitch adjustments would go here.
}
///////////////////////////////////////////////////////////////
