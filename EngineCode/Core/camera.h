///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Camera realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
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
	float m_fov;		 // Field of view
	float m_aspectRatio; // Aspect ratio
	float m_nearPlane;	 // Near clipping plane
	float m_farPlane;	 // Far clipping plane

	float m_yaw;   // Yaw angle (rotation around the Y axis)
	float m_pitch; // Pitch angle (rotation around the X axis)

	POINT m_ptLastMousePosition;

	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;

	// Smoothing parameters
	D3DXVECTOR3 m_targetPosition;
	D3DXVECTOR3 m_currentVelocity;
	float m_targetYaw;
	float m_targetPitch;
	float m_yawVelocity;
	float m_pitchVelocity;

	// Smoothing settings
	float m_positionSmoothTime;
	float m_rotationSmoothTime;
	float m_maxSpeed;

	void UpdateInput();
	void ApplySmoothing(float deltaTime);

  public:
	CCamera() = default;
	~CCamera() = default;

	void Initialize();
	void SetDefaultParams();
	void OnFrame();
	void ApplyMovement(D3DXVECTOR3 direction, float amount, float yawdelta, float pitchdelta);
	void CalculateMatrices();
	void Reset();

	void SetFov(float fov)
	{
		m_fov = D3DXToRadian(fov);
	}

	void SetFarPlane(float far_plane)
	{
		m_farPlane = far_plane;
	}

	// Smoothing settings
	void SetSmoothing(float positionSmoothTime = 0.1f, float rotationSmoothTime = 0.05f, float maxSpeed = 50.0f)
	{
		m_positionSmoothTime = positionSmoothTime;
		m_rotationSmoothTime = rotationSmoothTime;
		m_maxSpeed = maxSpeed;
	}

	D3DXMATRIX GetViewMatrix()
	{
		return m_View;
	}

	D3DXMATRIX GetProjectionMatrix()
	{
		return m_Projection;
	}
};
///////////////////////////////////////////////////////////////
extern CCamera* Camera;
///////////////////////////////////////////////////////////////
extern bool g_bNeedLockCursor;
extern bool g_bNeedUpdateCameraInput;
extern bool g_UseOrthogonalProjection;
extern float g_OrthogonalProjectionSize;
extern float g_Fov;
extern float g_Aspect;
extern float g_NearPlane;
extern float g_FarPlane;
///////////////////////////////////////////////////////////////
