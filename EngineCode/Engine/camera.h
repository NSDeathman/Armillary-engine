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

	// Camera settings
	float m_fov;			// Field of view
	float m_aspectRatio;	// Aspect ratio
	float m_nearPlane;		// Near clipping plane
	float m_farPlane;		// Far clipping plane

public:
	CCamera(D3DXVECTOR3 position, 
			D3DXVECTOR3 direction, 
			D3DXVECTOR3 upVec, 
			float fovDeg, 
			float aspect, 
			float nearPlane,
			float farPlane);
	~CCamera() = default;

	D3DXMATRIX GetViewMatrix();
	D3DXMATRIX GetProjectionMatrix();

	void Move(const D3DXVECTOR3& direction, float amount);
};
///////////////////////////////////////////////////////////////
extern CCamera* Camera;
///////////////////////////////////////////////////////////////
