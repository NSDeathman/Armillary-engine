#pragma once
#include <MathAPI/MathAPI.h>
#include "Core.h"

namespace Core::World
{
enum class ProjectionType
{
	Perspective,
	Orthographic
};

class CCamera
{
  public:
	CCamera();
	virtual ~CCamera();

	// --- Основные параметры ---
	void SetPosition(const Math::float3& pos);
	Math::float3 GetPosition() const;

	void SetRotation(const Math::quaternion& rot);
	Math::quaternion GetRotation() const;

	// --- Параметры проекции ---
	void SetPerspective(float fovDeg, float aspect, float zNear, float zFar);
	void SetOrthographic(float zNear, float zFar);
	void SetAspectRatio(float aspect);

	// --- Получение матриц ---
	Math::float4x4 GetViewMatrix() const;
	Math::float4x4 GetProjectionMatrix() const;
	Math::float4x4 GetViewProjection() const;

	// --- Векторы направлений ---
	Math::float3 GetForward() const;
	Math::float3 GetRight() const;
	Math::float3 GetUp() const;

	// --- Геттеры параметров камеры ---
	Math::float3 GetEye();
	float GetFOV();
	float GetAspectRatio();
	float GetNearPlane();
	float GetFarPlane();

	// Перемещение
	void TranslateLocal(const Math::float3& delta);
	void TranslateGlobal(const Math::float3& delta);

  protected:
	void UpdateVectors();

  private:
	Math::float3 m_Position;
	Math::quaternion m_Rotation;

	Math::float3 m_Eye;
	Math::float3 m_Forward;
	Math::float3 m_Right;
	Math::float3 m_Up;

	ProjectionType m_Type;
	float m_Fov;
	float m_AspectRatio;
	float m_Near;
	float m_Far;
};
} // namespace Core::World
