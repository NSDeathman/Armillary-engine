#include "stdafx.h"
#include "FlyingCameraControllerComponent.h"
#include "TransformComponent.h"
#include "Core.h"
#include <algorithm>
#include <cmath>

namespace Core::ECS::Components
{
FlyingCameraControllerComponent::FlyingCameraControllerComponent()
{
	m_TargetPosition = Math::float3::zero();
	m_SmoothedPosition = Math::float3::zero();
	m_TargetYaw = 0.0f;
	m_TargetPitch = 0.0f;
	m_SmoothedYaw = 0.0f;
	m_SmoothedPitch = 0.0f;
	m_IsControlling = false;
	m_IsFirstUpdate = true;
}

void FlyingCameraControllerComponent::OnCreate()
{
	auto* transform = m_Owner->Get<TransformComponent>();
	if (transform)
	{
		// Инициализируем из текущей трансформации
		Math::float3 euler = transform->GetLocalRotation().to_euler();
		m_TargetPitch = euler.x;
		m_TargetYaw = euler.y;
		m_TargetPosition = transform->GetLocalPosition();

		// Устанавливаем начальные сглаженные значения
		m_SmoothedPitch = m_TargetPitch;
		m_SmoothedYaw = m_TargetYaw;
		m_SmoothedPosition = m_TargetPosition;

		// Немедленно применяем начальные значения
		transform->SetPosition(m_SmoothedPosition);
		Math::quaternion rotation =
			Math::quaternion::rotation_y(m_SmoothedYaw) * Math::quaternion::rotation_x(m_SmoothedPitch);
		transform->SetRotation(rotation);
	}
}

void FlyingCameraControllerComponent::OnUpdate(float dt)
{
	if (!m_Owner || !m_Owner->IsActive() || dt <= 0.0f)
		return;

	auto* transform = m_Owner->Get<TransformComponent>();
	if (!transform)
		return;

	// Проверяем активность управления (ПКМ или геймпад)
	bool isRMBDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	float leftStickX = 0.0f, leftStickY = 0.0f;
	CoreAPI.Input.GetLeftStick(leftStickX, leftStickY);
	bool isGamepadActive = (std::abs(leftStickX) > GamepadDeadZone || std::abs(leftStickY) > GamepadDeadZone);

	bool wasControlling = m_IsControlling;
	m_IsControlling = isRMBDown || isGamepadActive;

	// Включение/выключение относительного режима мыши
	if (m_IsControlling && SDL_GetRelativeMouseMode() != SDL_TRUE)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else if (!m_IsControlling && SDL_GetRelativeMouseMode() == SDL_TRUE)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	if (m_IsControlling)
	{
		HandleRotation(dt, transform);
		HandleMovement(dt, transform);
	}
	else
	{
		// При отсутствии управления сбрасываем целевую скорость движения
		// Но продолжаем сглаживание к текущей позиции
	}

	// Всегда применяем сглаживание (как в старом коде)
	ApplySmoothing(dt, transform);

	m_IsFirstUpdate = false;
}

void FlyingCameraControllerComponent::HandleRotation(float dt, TransformComponent* transform)
{
	float yawDelta = 0.0f;
	float pitchDelta = 0.0f;

	// 1. Вращение мышью (только при зажатой ПКМ) - БОЛЕЕ ЧУВСТВИТЕЛЬНОЕ!
	if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0)
	{
		int mouseX = 0, mouseY = 0;
		SDL_GetRelativeMouseState(&mouseX, &mouseY);

		if (mouseX != 0 || mouseY != 0)
		{
			// ИСПРАВЛЕНИЕ: Увеличиваем чувствительность мыши (как в старом коде)
			yawDelta += mouseX * MouseSensitivity * 0.01f; // Было 0.001f, теперь 0.01f
			pitchDelta += (InvertY ? -mouseY : mouseY) * MouseSensitivity * 0.01f;
		}
	}

	// 2. Вращение геймпадом (правый стик) - оставляем как было
	float rightStickX = 0.0f, rightStickY = 0.0f;
	CoreAPI.Input.GetRightStick(rightStickX, rightStickY);

	if (std::abs(rightStickX) > GamepadDeadZone || std::abs(rightStickY) > GamepadDeadZone)
	{
		// Линейная зависимость для геймпада (проще и предсказуемее)
		yawDelta += rightStickX * GamepadSensitivity * dt;
		pitchDelta += (InvertY ? -rightStickY : rightStickY) * GamepadSensitivity * dt;
	}

	// 3. Обновляем целевые углы (как в старом коде)
	if (yawDelta != 0.0f || pitchDelta != 0.0f)
	{
		m_TargetYaw += yawDelta;
		m_TargetPitch += pitchDelta;

		// Ограничиваем pitch (как в старом коде)
		const float maxPitch = Math::Constants::HALF_PI - 0.01f;
		m_TargetPitch = Math::MathFunctions::clamp(m_TargetPitch, -maxPitch, maxPitch);
	}
}

void FlyingCameraControllerComponent::HandleMovement(float dt, TransformComponent* transform)
{
	Math::float3 moveDirection = Math::float3::zero();

	// Движение клавиатурой (как в старом коде)
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_W))
		moveDirection.z += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_S))
		moveDirection.z -= 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_A))
		moveDirection.x -= 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_D))
		moveDirection.x += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_E))
		moveDirection.y += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_Q))
		moveDirection.y -= 1.0f;

	// Движение геймпадом (как в старом коде)
	float leftStickX = 0.0f, leftStickY = 0.0f;
	CoreAPI.Input.GetLeftStick(leftStickX, leftStickY);

	if (std::abs(leftStickX) > GamepadDeadZone || std::abs(leftStickY) > GamepadDeadZone)
	{
		moveDirection.x += leftStickX;
		moveDirection.z -= leftStickY; // Обратите внимание на минус (как в старом коде)
	}

	// Вычисляем мировое направление движения
	if (!moveDirection.approximately_zero())
	{
		moveDirection = moveDirection.normalize();
		Math::quaternion rotation = transform->GetLocalRotation();
		Math::float3 worldDirection = rotation.transform_vector(moveDirection);

		// Регулировка скорости (как в старом коде)
		float currentSpeed = MoveSpeed * SpeedMultiplier;

		// Ускорение/замедление
		if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LSHIFT))
			currentSpeed *= 4.0f;
		else if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LCTRL))
			currentSpeed *= 0.25f;

		// Обновляем целевую позицию (как в старом коде)
		Math::float3 moveDelta = worldDirection * currentSpeed * dt;
		m_TargetPosition += moveDelta;
	}
}

void FlyingCameraControllerComponent::ApplySmoothing(float dt, TransformComponent* transform)
{
	// Применяем сглаживание вращения (как в старом коде)
	if (SmoothRotation)
	{
		float smoothTime = RotationSmoothTime;

		// При первом кадре мгновенно применяем значения
		if (m_IsFirstUpdate)
		{
			m_SmoothedYaw = m_TargetYaw;
			m_SmoothedPitch = m_TargetPitch;
		}
		else
		{
			// Экспоненциальное сглаживание (как в старом коде)
			m_SmoothedYaw = SmoothDamp(m_SmoothedYaw, m_TargetYaw, m_YawVelocity, smoothTime, dt);
			m_SmoothedPitch = SmoothDamp(m_SmoothedPitch, m_TargetPitch, m_PitchVelocity, smoothTime, dt);
		}

		// Применяем сглаженное вращение
		Math::quaternion rotation =
			Math::quaternion::rotation_y(m_SmoothedYaw) * Math::quaternion::rotation_x(m_SmoothedPitch);
		transform->SetRotation(rotation);
	}
	else
	{
		// Без сглаживания
		Math::quaternion rotation =
			Math::quaternion::rotation_y(m_TargetYaw) * Math::quaternion::rotation_x(m_TargetPitch);
		transform->SetRotation(rotation);
	}

	// Применяем сглаживание позиции (как в старом коде)
	if (SmoothMovement)
	{
		if (m_IsFirstUpdate)
		{
			m_SmoothedPosition = m_TargetPosition;
		}
		else
		{
			// SmoothDamp для позиции с ограничением максимальной скорости
			Math::float3 newPosition =
				SmoothDamp(m_SmoothedPosition, m_TargetPosition, m_PositionVelocity, PositionSmoothTime, dt);

			// Ограничиваем максимальную скорость (как в старом коде)
			float currentSpeed = m_PositionVelocity.length();
			if (currentSpeed > MaxSpeed)
			{
				m_PositionVelocity = m_PositionVelocity * (MaxSpeed / currentSpeed);
			}

			m_SmoothedPosition = newPosition;
		}

		transform->SetPosition(m_SmoothedPosition);
	}
	else
	{
		// Без сглаживания
		transform->SetPosition(m_TargetPosition);
	}
}

// Реализация SmoothDamp для float3 (как в Unity)
Math::float3 FlyingCameraControllerComponent::SmoothDamp(const Math::float3& current, const Math::float3& target,
														 Math::float3& currentVelocity, float smoothTime, float dt)
{
	// Ссылка: https://github.com/Unity-Technologies/UnityCsReference/blob/master/Runtime/Export/Math/Mathf.cs#L324
	float omega = 2.0f / smoothTime;
	float x = omega * dt;
	float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	Math::float3 change = current - target;
	Math::float3 temp = (currentVelocity + omega * change) * dt;
	currentVelocity = (currentVelocity - omega * temp) * exp;

	Math::float3 result = target + (change + temp) * exp;

	// Предотвращаем превышение цели
	if ((target - current).dot(result - target) > 0.0f)
	{
		result = target;
		currentVelocity = Math::float3::zero();
	}

	return result;
}

// Реализация SmoothDamp для float (как в Unity)
float FlyingCameraControllerComponent::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime,
												  float dt)
{
	float omega = 2.0f / smoothTime;
	float x = omega * dt;
	float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	float change = current - target;
	float temp = (currentVelocity + omega * change) * dt;
	currentVelocity = (currentVelocity - omega * temp) * exp;

	float result = target + (change + temp) * exp;

	// Предотвращаем превышение цели
	if ((target - current) * (result - target) > 0.0f)
	{
		result = target;
		currentVelocity = 0.0f;
	}

	return result;
}

std::unique_ptr<Core::ECS::IComponent> FlyingCameraControllerComponent::Clone() const
{
	auto clone = std::make_unique<FlyingCameraControllerComponent>();
	clone->MoveSpeed = MoveSpeed;
	clone->MouseSensitivity = MouseSensitivity;
	clone->GamepadSensitivity = GamepadSensitivity;
	clone->GamepadDeadZone = GamepadDeadZone;
	clone->InvertY = InvertY;
	clone->PositionSmoothTime = PositionSmoothTime;
	clone->RotationSmoothTime = RotationSmoothTime;
	clone->MaxSpeed = MaxSpeed;
	clone->SpeedMultiplier = SpeedMultiplier;
	clone->SmoothMovement = SmoothMovement;
	clone->SmoothRotation = SmoothRotation;

	// Копируем состояние
	clone->m_TargetPosition = m_TargetPosition;
	clone->m_TargetYaw = m_TargetYaw;
	clone->m_TargetPitch = m_TargetPitch;
	clone->m_SmoothedPosition = m_SmoothedPosition;
	clone->m_SmoothedYaw = m_SmoothedYaw;
	clone->m_SmoothedPitch = m_SmoothedPitch;
	clone->m_PositionVelocity = m_PositionVelocity;
	clone->m_YawVelocity = m_YawVelocity;
	clone->m_PitchVelocity = m_PitchVelocity;
	clone->m_IsControlling = m_IsControlling;
	clone->m_IsFirstUpdate = m_IsFirstUpdate;

	return clone;
}
} // namespace Core::ECS::Components
