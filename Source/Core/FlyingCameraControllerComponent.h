#pragma once
#include "ECSCore.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

namespace Core::ECS::Components
{
class FlyingCameraControllerComponent : public Core::ECS::Component<FlyingCameraControllerComponent>
{
  public:
	FlyingCameraControllerComponent();
	void OnCreate() override;
	void OnUpdate(float dt) override;

	// Настройки управления
	float MoveSpeed = 5.0f;
	float MouseSensitivity = 0.5f;
	float GamepadSensitivity = 2.0f;
	float GamepadDeadZone = 0.15f;
	bool InvertY = false;

	// Настройки сглаживания (взяты из старого кода)
	float PositionSmoothTime = 0.1f;  // Время сглаживания позиции
	float RotationSmoothTime = 0.05f; // Время сглаживания вращения
	float MaxSpeed = 50.0f;			  // Максимальная скорость движения

	// Дополнительные настройки
	float SpeedMultiplier = 1.0f; // Общий множитель скорости
	bool SmoothMovement = true;	  // Включить сглаживание движения
	bool SmoothRotation = true;	  // Включить сглаживание вращения

	bool IsControlling() const
	{
		return m_IsControlling;
	}
	std::unique_ptr<Core::ECS::IComponent> Clone() const override;

  private:
	void HandleRotation(float dt, TransformComponent* transform);
	void HandleMovement(float dt, TransformComponent* transform);
	void ApplySmoothing(float dt, TransformComponent* transform);
	Math::float3 SmoothDamp(const Math::float3& current, const Math::float3& target, Math::float3& currentVelocity, float smoothTime, float dt);
	float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float dt);

	// Целевые значения для сглаживания
	Math::float3 m_TargetPosition;
	float m_TargetYaw = 0.0f;
	float m_TargetPitch = 0.0f;

	// Текущие сглаженные значения
	Math::float3 m_SmoothedPosition;
	float m_SmoothedYaw = 0.0f;
	float m_SmoothedPitch = 0.0f;

	// Векторы скорости для сглаживания
	Math::float3 m_PositionVelocity = Math::float3::zero();
	float m_YawVelocity = 0.0f;
	float m_PitchVelocity = 0.0f;

	// Состояние управления
	bool m_IsControlling = false;
	bool m_IsFirstUpdate = true;
};
} // namespace Core::ECS::Components
