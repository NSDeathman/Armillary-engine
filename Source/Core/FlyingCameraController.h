#pragma once
#include "CameraController.h"

namespace Core::World
{
class  CFlyingCameraController : public ICameraController
{
  public:
	CFlyingCameraController();
	virtual ~CFlyingCameraController();

	void Update(CCamera& camera, float dt) override;

	// Public настройки
	float MoveSpeed;
	float SprintMultiplier;
	float MouseSensitivity;
	float GamepadSensitivity;
	float Smoothness;
	float MovementSmoothness;

	bool m_EnableResponseCurve = true;
	bool m_EnableSlopeAdjustment = false;
	bool m_ShowDebugInfo = false; 

  private:
	float m_Yaw;
	float m_Pitch;
	Math::float3 m_LastMovementDirection;

	void HandleRotation(CCamera& camera, float dt);
	void HandleMovement(CCamera& camera, float dt);

	Math::float3 ApplyMovementCurve(const Math::float3& velocity);
	float CalculateSlopeFactor(const Math::float3& direction);
	void ApplyCameraBobbing(CCamera& camera, float dt);
	void ApplyHeadBob(CCamera& camera, float dt);
};
} // namespace Core::World
