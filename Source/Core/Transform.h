#pragma once
#include <MathAPI/MathAPI.h>

namespace Core::World
{
struct Transform
{
	// Используем типы из вашей библиотеки
	Math::float3 Position = Math::float3::zero();
	Math::quaternion Rotation = Math::quaternion::identity();
	Math::float3 Scale = Math::float3::one();

	// Получение матрицы модели с использованием оптимизированной функции TRS из MathAPI
	Math::float4x4 GetMatrix() const
	{
		return Math::float4x4::TRS(Position, Rotation, Scale);
	}

	void SetPosition(float x, float y, float z)
	{
		Position = Math::float3(x, y, z);
	}

	void SetScale(float s)
	{
		Scale = Math::float3(s);
	}

	// Вращение через углы Эйлера (в градусах для удобства, внутри переводим в радианы)
	void SetRotation(float pitchDeg, float yawDeg, float rollDeg)
	{
		// Используем FastMath для перевода градусов в радианы
		float p = Math::FastMath::to_radians(pitchDeg);
		float y = Math::FastMath::to_radians(yawDeg);
		float r = Math::FastMath::to_radians(rollDeg);

		Rotation = Math::quaternion::from_euler(p, y, r);
	}

	// Сдвиг относительно текущего поворота (Local Move)
	void TranslateLocal(const Math::float3& delta)
	{
		// transform_vector поворачивает вектор без учета масштаба
		Position += Rotation.transform_vector(delta);
	}
};
} // namespace Core::World
