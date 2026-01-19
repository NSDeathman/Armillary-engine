#pragma once
#include "ECSCore.h"
#include <MathAPI/MathAPI.h>
#include "TransformComponent.h"

namespace Core::ECS::Components
{
enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

class CameraComponent : public Core::ECS::Component<CameraComponent>
{
  public:
	CameraComponent();
	virtual ~CameraComponent() = default;

	// --- Основные параметры камеры ---
	void SetPerspective(float fovDeg, float aspect, float zNear, float zFar);
	void SetOrthographic(float size, float zNear, float zFar);
	void SetAspectRatio(float aspect);

	// Геттеры параметров проекции
	float GetFOV() const
	{
		return m_Fov;
	}
	float GetAspectRatio() const
	{
		return m_AspectRatio;
	}
	float GetNearPlane() const
	{
		return m_Near;
	}
	float GetFarPlane() const
	{
		return m_Far;
	}
	CameraProjectionType GetProjectionType() const
	{
		return m_Type;
	}

	// --- Матрицы ---
	Math::float4x4 GetViewMatrix() const;
	Math::float4x4 GetProjectionMatrix() const;
	Math::float4x4 GetViewProjectionMatrix() const;

	// --- Векторы направлений (из Transform) ---
	Math::float3 GetForward() const;
	Math::float3 GetRight() const;
	Math::float3 GetUp() const;

	// --- Геттер позиции камеры ---
	Math::float3 GetPosition() const;

	// --- Утилиты ---
	bool IsMainCamera() const
	{
		return m_IsMainCamera;
	}
	void SetMainCamera(bool isMain)
	{
		m_IsMainCamera = isMain;
	}

	std::unique_ptr<Core::ECS::IComponent> Clone() const override;

  private:
	CameraProjectionType m_Type = CameraProjectionType::Perspective;
	float m_Fov = Math::Constants::PI / 3.0f; // 60 градусов
	float m_AspectRatio = 16.0f / 9.0f;
	float m_Near = 0.1f;
	float m_Far = 1000.0f;
	float m_OrthoSize = 10.0f;

	bool m_IsMainCamera = false;

	// Кэшированные матрицы
	mutable Math::float4x4 m_ProjectionMatrix;
	mutable bool m_ProjectionDirty = true;

	// Метод для обновления проекционной матрицы
	void UpdateProjectionMatrix() const;
};
} // namespace Core::ECS::Components
