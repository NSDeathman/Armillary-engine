///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#include "camera.h"
#include "log.h"
///////////////////////////////////////////////////////////////
	// Constructor
CCamera::CCamera(D3DXVECTOR3 position, 
				D3DXVECTOR3 direction, 
				D3DXVECTOR3 upVec, 
				float fovDeg, 
				float aspect, 
				float nearPlane,
				float farPlane)
{
	m_position = position;
	m_direction = direction;
	m_upVec = upVec;
	m_fov = D3DXToRadian(fovDeg);
	m_aspectRatio = aspect; 
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
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
	return projectionMatrix;
}

// Update camera position
void CCamera::Move(const D3DXVECTOR3& direction, float amount)
{
	m_position += direction * amount;

	// Keep the target relative to the camera position
	m_direction += direction * amount;
}

// Rotate camera around the target point (simple placeholder)
void rotate(float yaw, float pitch)
{
	// Here you could implement additional calculations to alter the target based on yaw/pitch
	// For simplicity, this is not implemented.
}
///////////////////////////////////////////////////////////////
