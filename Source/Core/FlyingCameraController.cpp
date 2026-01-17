#include "stdafx.h"
#include "FlyingCameraController.h"
#include "Input.h"
#include <algorithm>

namespace Core::World
{
CFlyingCameraController::CFlyingCameraController()
	: m_Yaw(0.0f), m_Pitch(0.0f), MoveSpeed(5.0f), SprintMultiplier(2.5f), MouseSensitivity(0.5f),
	  GamepadSensitivity(2.0f), Smoothness(10.0f), MovementSmoothness(8.0f),
	  m_LastMovementDirection(Math::float3::forward())
{
}

CFlyingCameraController::~CFlyingCameraController()
{
}

void CFlyingCameraController::Update(CCamera& camera, float dt)
{
	// Обработка вращения с плавностью
	HandleRotation(camera, dt);

	// Обработка движения с плавностью
	HandleMovement(camera, dt);

	// Дополнительные эффекты (опционально)
	//ApplyCameraBobbing(camera, dt); // Эффект шагания
	//ApplyHeadBob(camera, dt);		// Эффект тряски головы при беге
}

void CFlyingCameraController::HandleRotation(CCamera& camera, float dt)
{
	// Используем сглаженный ввод мыши
	int mouseDx = 0, mouseDy = 0;
	CoreAPI.Input.GetMouseDelta(mouseDx, mouseDy);

	// Используем сглаженный ввод геймпада
	float stickX = 0.0f, stickY = 0.0f;
	CoreAPI.Input.GetRightStick(stickX, stickY);

	// Комбинируем ввод от мыши и геймпада
	float rotY = (float)mouseDx * MouseSensitivity + stickX * GamepadSensitivity;
	float rotX = (float)mouseDy * MouseSensitivity + stickY * GamepadSensitivity;

	// Применяем вращение с плавностью
	static float smoothedRotY = 0.0f;
	static float smoothedRotX = 0.0f;

	if (Smoothness > 0.0f && dt > 0.0f)
	{
		float alpha = 1.0f - expf(-dt * Smoothness);
		smoothedRotY = Math::lerp(smoothedRotY, rotY, alpha);
		smoothedRotX = Math::lerp(smoothedRotX, rotX, alpha);

		rotY = smoothedRotY;
		rotX = smoothedRotX;
	}

	// Применяем вращение
	if (fabsf(rotY) > 0.001f || fabsf(rotX) > 0.001f)
	{
		m_Yaw += rotY;
		m_Pitch += rotX;

		m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

		float yawRad = Math::FastMath::to_radians(m_Yaw);
		float pitchRad = Math::FastMath::to_radians(m_Pitch);

		Math::quaternion qYaw = Math::quaternion::rotation_y(yawRad);
		Math::quaternion qPitch = Math::quaternion::rotation_x(pitchRad);

		Math::quaternion finalRot = qYaw * qPitch;
		camera.SetRotation(finalRot);
	}
}

void CFlyingCameraController::HandleMovement(CCamera& camera, float dt)
{
	// Минимальная delta time для избежания деления на ноль
	if (dt <= 0.0f)
		return;

	// Целевая скорость на основе ввода
	Math::float3 targetVelocity = Math::float3::zero();

	// --- Клавиатура ---
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_W))
		targetVelocity.z += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_S))
		targetVelocity.z -= 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_A))
		targetVelocity.x -= 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_D))
		targetVelocity.x += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_E))
		targetVelocity.y += 1.0f;
	if (CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_Q))
		targetVelocity.y -= 1.0f;

	// --- Геймпад ---
	float padX = 0.0f, padY = 0.0f;
	CoreAPI.Input.GetLeftStick(padX, padY);
	targetVelocity.x += padX;
	targetVelocity.z -= padY; // Инвертируем ось Y для соответствия клавиатуре

	float trLeft = CoreAPI.Input.GetLeftTrigger();
	float trRight = CoreAPI.Input.GetRightTrigger();
	targetVelocity.y += (trRight - trLeft);

	// Нормализация вектора скорости при диагональном движении
	if (targetVelocity.length_sq() > 1.0f)
	{
		targetVelocity = targetVelocity.normalize();
	}

	// Ускорение (спринт)
	float targetSpeed = MoveSpeed;
	bool isSprinting = CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LSHIFT) || CoreAPI.Input.IsGamepadButtonHeld(SDL_CONTROLLER_BUTTON_LEFTSTICK);

	if (isSprinting)
	{
		targetSpeed *= SprintMultiplier;
	}

	// Масштабируем целевую скорость
	targetVelocity *= targetSpeed;

	// ========== ПЛАВНОЕ ДВИЖЕНИЕ С ИНЕРЦИЕЙ ==========

	// Состояния для плавности движения
	static Math::float3 currentVelocity = Math::float3::zero();
	static Math::float3 velocitySmoothing = Math::float3::zero();
	static float currentSpeed = MoveSpeed;
	static float speedSmoothing = 0.0f;

	// Коэффициент плавности для скорости (можно сделать настраиваемым)
	float velocitySmoothFactor = 10.0f; // Высокое значение = быстрый отклик
	float speedSmoothFactor = 5.0f;		// Для плавного изменения скорости

	// Плавный переход скорости
	float alphaSpeed = 1.0f - expf(-dt * speedSmoothFactor);
	currentSpeed = Math::lerp(currentSpeed, targetSpeed, alphaSpeed);

	// Плавный переход вектора скорости (экспоненциальное сглаживание)
	// Используем разные коэффициенты для разных осей при необходимости
	float alphaX = 1.0f - expf(-dt * velocitySmoothFactor);
	float alphaY = 1.0f - expf(-dt * velocitySmoothFactor * 0.7f); // Медленнее по Y
	float alphaZ = 1.0f - expf(-dt * velocitySmoothFactor);

	// Применяем сглаживание к каждой компоненте
	currentVelocity.x = Math::lerp(currentVelocity.x, targetVelocity.x, alphaX);
	currentVelocity.y = Math::lerp(currentVelocity.y, targetVelocity.y, alphaY);
	currentVelocity.z = Math::lerp(currentVelocity.z, targetVelocity.z, alphaZ);

	// Применяем окончательную скорость после сглаживания
	Math::float3 finalVelocity = currentVelocity;

	// Если движение практически остановилось, полностью сбрасываем скорость
	if (finalVelocity.length_sq() < 0.001f)
	{
		finalVelocity = Math::float3::zero();
		currentVelocity = Math::float3::zero();
	}

	// ========== ДОПОЛНИТЕЛЬНЫЕ УЛУЧШЕНИЯ ==========

	// Вычисляем смещение с учетом delta time и плавной скорости
	Math::float3 deltaMove = finalVelocity * dt;

	// Применяем движение с учетом плавности
	if (deltaMove.length_sq() > 0.000001f) // Проверка на минимальное движение
	{
		// Движение: X и Z локально, Y глобально (как в оригинале)
		camera.TranslateLocal(Math::float3(deltaMove.x, 0, deltaMove.z));
		camera.TranslateGlobal(Math::float3(0, deltaMove.y, 0));

		// Сохраняем последнее значимое движение для инерции
		m_LastMovementDirection = finalVelocity.normalize();
	}

// ========== ДЕБАГ ИНФОРМАЦИЯ ==========
#ifdef _DEBUG
	if (m_ShowDebugInfo)
	{
		Print("Movement Debug:");
		Print("  Target Velocity: (%.2f, %.2f, %.2f)", targetVelocity.x, targetVelocity.y, targetVelocity.z);
		Print("  Current Velocity: (%.2f, %.2f, %.2f)", currentVelocity.x, currentVelocity.y, currentVelocity.z);
		Print("  Speed: %.2f / %.2f (current/target)", currentSpeed, targetSpeed);
		Print("  Delta Move: (%.4f, %.4f, %.4f)", deltaMove.x, deltaMove.y, deltaMove.z);
	}
#endif
}

void CFlyingCameraController::ApplyCameraBobbing(CCamera& camera, float dt)
{
	// Эффект покачивания камеры при движении
	static float bobTime = 0.0f;
	static float bobAmount = 0.0f;

	// Вычисляем силу шагания на основе скорости
	Math::float3 velocity;
	// ... получить текущую скорость ...

	float speed = velocity.length();
	float targetBobAmount = speed > 0.1f ? std::min(speed / MoveSpeed, 1.0f) * 0.1f : 0.0f;

	// Плавное изменение амплитуды
	float alpha = 1.0f - expf(-dt * 5.0f);
	bobAmount = Math::lerp(bobAmount, targetBobAmount, alpha);

	if (bobAmount > 0.001f)
	{
		bobTime += dt * speed * 10.0f;

		// Синусоидальное движение для эффекта шага
		float bobX = sinf(bobTime * 2.0f) * bobAmount * 0.5f;
		float bobY = fabsf(sinf(bobTime)) * bobAmount;

		// Применяем небольшое смещение к камере
		camera.TranslateLocal(Math::float3(bobX, bobY, 0));
	}
}

void CFlyingCameraController::ApplyHeadBob(CCamera& camera, float dt)
{
	// Эффект тряски головы при спринте
	static float trauma = 0.0f; // "Травма" - уровень тряски

	// Увеличиваем травму при спринте
	bool isSprinting = CoreAPI.Input.IsKeyHeld(SDL_SCANCODE_LSHIFT) || CoreAPI.Input.IsGamepadButtonHeld(SDL_CONTROLLER_BUTTON_LEFTSTICK);

	if (isSprinting)
	{
		trauma = std::min(trauma + dt * 0.5f, 1.0f);
	}
	else
	{
		trauma = std::max(trauma - dt * 0.3f, 0.0f);
	}

	if (trauma > 0.01f)
	{
		// Эффект тряски на основе шума Перлина
		float time = CoreAPI.TimeSystem.GetTotalTime();
		float shake = trauma * trauma; // Квадрат для более интенсивной тряски

		float offsetX = (sinf(time * 15.7f) + sinf(time * 13.3f) * 0.5f) * shake * 0.05f;
		float offsetY = (cosf(time * 14.3f) + cosf(time * 11.7f) * 0.5f) * shake * 0.05f;
		float offsetZ = (sinf(time * 16.1f) + cosf(time * 12.9f) * 0.5f) * shake * 0.02f;

		// Применяем небольшие вращения для эффекта тряски
		Math::quaternion shakeRot = Math::quaternion::from_euler(Math::float3(offsetY, offsetX, offsetZ));

		// Комбинируем с текущим вращением
		Math::quaternion currentRot = camera.GetRotation();
		camera.SetRotation(currentRot * shakeRot);
	}
}

} // namespace Core::World
