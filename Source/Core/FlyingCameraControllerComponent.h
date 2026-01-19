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

	void OnCreate();

	// Жизненный цикл компонента
	void OnUpdate(float dt) override;

	// Настройки
	float MoveSpeed = 5.0f;
	float SprintMultiplier = 2.5f;
	float MouseSensitivity = 0.5f;
	float GamepadSensitivity = 2.0f;
	float Smoothness = 10.0f;		 // Для плавности вращения
	float MovementSmoothness = 8.0f; // Для плавности движения

	bool EnableResponseCurve = true;
	bool EnableSlopeAdjustment = false;
	bool ShowDebugInfo = false;

	std::unique_ptr<Core::ECS::IComponent> Clone() const override;

  private:
	void HandleRotation(float dt, TransformComponent* transform);
	void HandleMovement(float dt, TransformComponent* transform);
	void ApplyCameraBobbing(float dt, TransformComponent* transform);
	void ApplyHeadBob(float dt, TransformComponent* transform);

	// Состояние вращения
	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;
	float m_SmoothedRotY = 0.0f;
	float m_SmoothedRotX = 0.0f;

	// Состояние движения
	Math::float3 m_CurrentVelocity = Math::float3::zero();
	float m_CurrentSpeed = 5.0f;
	Math::float3 m_LastMovementDirection = Math::float3::forward();

	// Для эффектов
	float m_BobTime = 0.0f;
	float m_BobAmount = 0.0f;
	float m_Trauma = 0.0f;

	struct MouseFilter
	{
		std::array<float, 5> historyYaw;
		std::array<float, 5> historyPitch;
		int index = 0;

		MouseFilter()
		{
			historyYaw.fill(0.0f);
			historyPitch.fill(0.0f);
		}

		void AddSample(float yaw, float pitch)
		{
			historyYaw[index] = yaw;
			historyPitch[index] = pitch;
			index = (index + 1) % historyYaw.size();
		}

		std::pair<float, float> GetFiltered()
		{
			float avgYaw = 0.0f;
			float avgPitch = 0.0f;

			for (size_t i = 0; i < historyYaw.size(); ++i)
			{
				avgYaw += historyYaw[i];
				avgPitch += historyPitch[i];
			}

			avgYaw /= historyYaw.size();
			avgPitch /= historyPitch.size();

			return {avgYaw, avgPitch};
		}
	};

	MouseFilter m_MouseFilter;
};
} // namespace Core::ECS::Components
