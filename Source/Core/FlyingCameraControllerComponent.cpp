#include "stdafx.h"
#include "FlyingCameraControllerComponent.h"
#include "TransformComponent.h"
#include "Core.h"
#include <algorithm>

namespace Core::ECS::Components
{
FlyingCameraControllerComponent::FlyingCameraControllerComponent()
{
	// Инициализация
	m_Yaw = 0.0f;
	m_Pitch = 0.0f;
	m_SmoothedRotY = 0.0f;
	m_SmoothedRotX = 0.0f;
	m_CurrentVelocity = Math::float3::zero();
	m_CurrentSpeed = MoveSpeed;
	m_LastMovementDirection = Math::float3(0, 0, 1); // forward
	m_BobTime = 0.0f;
	m_BobAmount = 0.0f;
	m_Trauma = 0.0f;
}

void FlyingCameraControllerComponent::OnCreate()
{
	auto* transform = m_Owner->Get<TransformComponent>();
	if (transform)
	{
		// Инициализируем углы из текущего вращения камеры
		Math::float3 euler = transform->GetLocalRotation().to_euler();
		m_Pitch = Math::FastMath::to_degrees(euler.x);
		m_Yaw = Math::FastMath::to_degrees(euler.y);
	}
}

void FlyingCameraControllerComponent::OnUpdate(float dt)
{
	if (!m_Owner || !m_Owner->IsActive())
		return;

	// Проверяем, есть ли необходимые компоненты
	auto* transform = m_Owner->Get<TransformComponent>();
	if (!transform)
		return;

	// Проверяем, зажата ли правая кнопка мыши для управления
	bool isRMBDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

	if (isRMBDown)
	{
		// Включаем относительный режим мыши
		if (SDL_GetRelativeMouseMode() != SDL_TRUE)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
			// Даем SDL один кадр на активацию режима
			return;
		}

		HandleRotation(dt, transform);
		HandleMovement(dt, transform);

		// Опциональные эффекты
		if (EnableResponseCurve)
			ApplyCameraBobbing(dt, transform);
		if (EnableSlopeAdjustment)
			ApplyHeadBob(dt, transform);
	}
	else
	{
		// Выключаем относительный режим мыши
		if (SDL_GetRelativeMouseMode() == SDL_TRUE)
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}

		// Сбрасываем состояние плавности при отпускании кнопки
		m_CurrentVelocity = Math::float3::zero();

		// Не сбрасываем углы вращения - камера должна сохранять ориентацию
	}
}

void FlyingCameraControllerComponent::HandleRotation(float dt, TransformComponent* transform)
{
	// Получаем ввод мыши
	int mouseDx = 0, mouseDy = 0;

	// Всегда используем относительное движение, если режим включен
	if (SDL_GetRelativeMouseMode() == SDL_TRUE)
	{
		SDL_GetRelativeMouseState(&mouseDx, &mouseDy);
	}

	// Если дельта слишком большая (рывок), игнорируем её
	const int MAX_DELTA_PER_FRAME = 50; // Максимальная разумная дельта за кадр
	if (abs(mouseDx) > MAX_DELTA_PER_FRAME || abs(mouseDy) > MAX_DELTA_PER_FRAME)
	{
		Print("Ignoring mouse spike: dx=%d, dy=%d", mouseDx, mouseDy);
		mouseDx = 0;
		mouseDy = 0;
	}

	// Получаем ввод геймпада
	float stickX = 0.0f, stickY = 0.0f;
	CoreAPI.Input.GetRightStick(stickX, stickY);

	// Комбинируем ввод
	float yawDelta = (float)mouseDx * MouseSensitivity;
	float pitchDelta = (float)mouseDy * MouseSensitivity;

	yawDelta += stickX * GamepadSensitivity;
	pitchDelta += stickY * GamepadSensitivity;

	// Применяем фильтр сглаживания
	m_MouseFilter.AddSample(yawDelta, pitchDelta);
	auto filtered = m_MouseFilter.GetFiltered();

	yawDelta = filtered.first;
	pitchDelta = filtered.second;

	// Применяем с учетом времени кадра
	yawDelta *= dt * 60.0f;
	pitchDelta *= dt * 60.0f;

	// Получаем текущие углы Эйлера из вращения камеры
	Math::quaternion currentRot = transform->GetLocalRotation();
	Math::float3 currentEuler = currentRot.to_euler();

	// Текущие углы в градусах
	float currentYawDeg = Math::FastMath::to_degrees(currentEuler.y);
	float currentPitchDeg = Math::FastMath::to_degrees(currentEuler.x);

	// Обновляем углы
	float newYawDeg = currentYawDeg + yawDelta;
	float newPitchDeg = currentPitchDeg + pitchDelta;

	// Ограничиваем pitch
	newPitchDeg = std::clamp(newPitchDeg, -89.0f, 89.0f);

	// Создаем новое вращение из углов Эйлера
	Math::quaternion newRotation =
		Math::quaternion::from_euler(Math::FastMath::to_radians(newPitchDeg), Math::FastMath::to_radians(newYawDeg),
									 Math::FastMath::to_radians(0.0f));

	// Применяем вращение
	transform->SetRotation(newRotation);

	// Отладочный вывод
	if (ShowDebugInfo)
	{
		static int frameCount = 0;
		frameCount++;

		if (frameCount % 10 == 0) // Каждый 10-й кадр
		{
			Print("Camera Rotation Debug:");
			Print("  Mouse Delta: dx=%d, dy=%d", mouseDx, mouseDy);
			Print("  Processed Delta: Yaw=%.2f, Pitch=%.2f", yawDelta, pitchDelta);
			Print("  dt=%.4f, FPS=%.1f", dt, 1.0f / dt);
			Print("  New Angles: Yaw=%.2f°, Pitch=%.2f°", newYawDeg, newPitchDeg);
		}
	}
}

void FlyingCameraControllerComponent::HandleMovement(float dt, TransformComponent* transform)
{
	if (dt <= 0.0f)
		return;

	// Получаем локальные оси из текущего вращения
	Math::quaternion rotation = transform->GetLocalRotation();

	// Локальные направления (в системе координат камеры)
	// В левосторонней системе:
	// - forward: ось Z+ (0, 0, 1)
	// - right: ось X+ (1, 0, 0)
	// - up: ось Y+ (0, 1, 0)
	Math::float3 localForward = Math::float3(0, 0, 1);
	Math::float3 localRight = Math::float3(1, 0, 0);
	Math::float3 localUp = Math::float3(0, 1, 0);

	// Преобразуем в мировые направления
	Math::float3 forward = rotation.transform_vector(localForward);
	Math::float3 right = rotation.transform_vector(localRight);
	Math::float3 up = rotation.transform_vector(localUp);

	// Нормализуем (на всякий случай)
	forward = forward.normalize();
	right = right.normalize();
	up = up.normalize();

	// Вычисляем направление движения
	Math::float3 moveDirection = Math::float3::zero();

	// Обработка ввода
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_W))
		moveDirection += forward;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_S))
		moveDirection -= forward;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_A))
		moveDirection -= right;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_D))
		moveDirection += right;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_E))
		moveDirection += up;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_Q))
		moveDirection -= up;

	// Если есть направление движения, нормализуем
	float moveLength = moveDirection.length();
	if (moveLength > 0.0001f)
	{
		moveDirection = moveDirection / moveLength;

		// Скорость
		float currentSpeed = MoveSpeed;
		if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LSHIFT))
			currentSpeed *= SprintMultiplier;

		// Применяем движение к позиции
		Math::float3 position = transform->GetLocalPosition();
		position += moveDirection * (currentSpeed * dt);
		transform->SetPosition(position);

		// Отладочный вывод
		//if (ShowDebugInfo)
		{
			Print("Camera Movement Debug:");
			Print("  Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
			Print("  Move Dir: (%.2f, %.2f, %.2f)", moveDirection.x, moveDirection.y, moveDirection.z);
			Print("  Forward:  (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
		}
	}
}

void FlyingCameraControllerComponent::ApplyCameraBobbing(float dt, TransformComponent* transform)
{
	float speed = m_CurrentVelocity.length();
	float targetBobAmount = speed > 0.1f ? std::min(speed / MoveSpeed, 1.0f) * 0.05f : 0.0f;

	float alpha = 1.0f - expf(-dt * 5.0f);
	m_BobAmount = Math::lerp(m_BobAmount, targetBobAmount, alpha);

	if (m_BobAmount > 0.001f)
	{
		m_BobTime += dt * speed * 10.0f;
		float bobX = sinf(m_BobTime * 2.0f) * m_BobAmount * 0.5f;
		float bobY = fabsf(sinf(m_BobTime)) * m_BobAmount;

		// Применяем смещение к позиции (в локальном пространстве)
		Math::float3 pos = transform->GetLocalPosition();
		pos.x += bobX;
		pos.y += bobY;
		transform->SetPosition(pos);
	}
}

void FlyingCameraControllerComponent::ApplyHeadBob(float dt, TransformComponent* transform)
{
	bool isSprinting = CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LSHIFT) ||
					   CoreAPI.Input.IsGamepadButtonHeld(SDL_CONTROLLER_BUTTON_LEFTSTICK);

	if (isSprinting)
		m_Trauma = std::min(m_Trauma + dt * 0.5f, 1.0f);
	else
		m_Trauma = std::max(m_Trauma - dt * 0.3f, 0.0f);

	if (m_Trauma > 0.01f)
	{
		float time = CoreAPI.TimeSystem.GetTotalTime();
		float shake = m_Trauma * m_Trauma;

		float offsetX = (sinf(time * 15.7f) + sinf(time * 13.3f) * 0.5f) * shake * 0.05f;
		float offsetY = (cosf(time * 14.3f) + cosf(time * 11.7f) * 0.5f) * shake * 0.05f;

		Math::quaternion currentRot = transform->GetLocalRotation();
		Math::quaternion shakeRot = Math::quaternion::from_euler(Math::float3(offsetY, offsetX, 0));
		transform->SetRotation(currentRot * shakeRot);
	}
}

std::unique_ptr<Core::ECS::IComponent> FlyingCameraControllerComponent::Clone() const
{
	auto clone = std::make_unique<FlyingCameraControllerComponent>();
	clone->MoveSpeed = MoveSpeed;
	clone->SprintMultiplier = SprintMultiplier;
	clone->MouseSensitivity = MouseSensitivity;
	clone->GamepadSensitivity = GamepadSensitivity;
	clone->Smoothness = Smoothness;
	clone->MovementSmoothness = MovementSmoothness;
	clone->EnableResponseCurve = EnableResponseCurve;
	clone->EnableSlopeAdjustment = EnableSlopeAdjustment;
	clone->ShowDebugInfo = ShowDebugInfo;
	clone->m_Yaw = m_Yaw;
	clone->m_Pitch = m_Pitch;
	clone->m_SmoothedRotY = m_SmoothedRotY;
	clone->m_SmoothedRotX = m_SmoothedRotX;
	clone->m_CurrentVelocity = m_CurrentVelocity;
	clone->m_CurrentSpeed = m_CurrentSpeed;
	clone->m_LastMovementDirection = m_LastMovementDirection;
	clone->m_BobTime = m_BobTime;
	clone->m_BobAmount = m_BobAmount;
	clone->m_Trauma = m_Trauma;
	return clone;
}
} // namespace Core::ECS::Components
