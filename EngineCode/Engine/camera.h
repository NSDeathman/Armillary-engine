///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <d3dx9.h>
///////////////////////////////////////////////////////////////
class CCamera
{
private:
	// Camera attributes
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR3 m_upVec;

	D3DXVECTOR3 m_moveDirection;

	// Camera settings
	float m_fov;			// Field of view
	float m_aspectRatio;	// Aspect ratio
	float m_nearPlane;		// Near clipping plane
	float m_farPlane;		// Far clipping plane

	float m_yaw;   // Yaw angle (rotation around the Y axis)
	float m_pitch; // Pitch angle (rotation around the X axis)

public:
	CCamera() = default;
	~CCamera() = default;

	void Initialize();
	void OnFrame();
	void Reset();

	D3DXMATRIX GetViewMatrix();
	D3DXMATRIX GetProjectionMatrix();

	void UpdateInput();
	void Move(const D3DXVECTOR3& direction, float amount);
};
///////////////////////////////////////////////////////////////
extern CCamera* Camera;
///////////////////////////////////////////////////////////////
extern bool g_UseOrthogonalProjection;
extern float g_OrthogonalProjectionSize;
extern float g_Fov;
extern float g_Aspect;
extern float g_NearPlane;
extern float g_FarPlane;
///////////////////////////////////////////////////////////////
