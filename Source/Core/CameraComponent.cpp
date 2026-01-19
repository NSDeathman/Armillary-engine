#include "stdafx.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

namespace Core::ECS::Components
{
CameraComponent::CameraComponent()
{
	// Инициализируем проекционную матрицу единичной матрицей
	m_ProjectionMatrix = Math::float4x4::identity();
	m_ProjectionDirty = true;
}

void CameraComponent::SetPerspective(float fovDeg, float aspect, float zNear, float zFar)
{
	m_Type = CameraProjectionType::Perspective;
	m_Fov = Math::FastMath::to_radians(fovDeg);
	m_AspectRatio = aspect;
	m_Near = zNear;
	m_Far = zFar;
	m_ProjectionDirty = true;

	// Сразу обновляем матрицу
	UpdateProjectionMatrix();
}

void CameraComponent::SetOrthographic(float size, float zNear, float zFar)
{
	m_Type = CameraProjectionType::Orthographic;
	m_OrthoSize = size;
	m_Near = zNear;
	m_Far = zFar;
	m_ProjectionDirty = true;
	UpdateProjectionMatrix();
}

void CameraComponent::SetAspectRatio(float aspect)
{
	m_AspectRatio = aspect;
	m_ProjectionDirty = true;
	UpdateProjectionMatrix();
}

Math::float4x4 CameraComponent::GetViewMatrix() const
{
	if (!m_Owner)
		return Math::float4x4::identity();

	auto* transform = m_Owner->Get<TransformComponent>();
	if (!transform)
		return Math::float4x4::identity();

	const Math::float3 position = transform->GetWorldPosition();
	const Math::float3 forward = GetForward();
	const Math::float3 up = GetUp();
	const Math::float3 target = position + forward;

	// Проверка на нулевые векторы
	if (forward.length_sq() < 0.0001f || up.length_sq() < 0.0001f)
		return Math::float4x4::identity();

	return Math::float4x4::look_at_lh(position, target, up);
}

Math::float4x4 CameraComponent::GetProjectionMatrix() const
{
	if (m_ProjectionDirty)
	{
		UpdateProjectionMatrix();
	}
	return m_ProjectionMatrix;
}

Math::float4x4 CameraComponent::GetViewProjectionMatrix() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}

Math::float3 CameraComponent::GetForward() const
{
	if (!m_Owner)
		return Math::float3(0, 0, 1); // Z-forward

	auto* transform = m_Owner->Get<TransformComponent>();
	return transform ? transform->GetForward() : Math::float3(0, 0, 1);
}

Math::float3 CameraComponent::GetRight() const
{
	if (!m_Owner)
		return Math::float3(1, 0, 0); // X-right

	auto* transform = m_Owner->Get<TransformComponent>();
	return transform ? transform->GetRight() : Math::float3(1, 0, 0);
}

Math::float3 CameraComponent::GetUp() const
{
	if (!m_Owner)
		return Math::float3(0, 1, 0); // Y-up

	auto* transform = m_Owner->Get<TransformComponent>();
	return transform ? transform->GetUp() : Math::float3(0, 1, 0);
}

Math::float3 CameraComponent::GetPosition() const
{
	if (!m_Owner)
		return Math::float3::zero();

	auto* transform = m_Owner->Get<TransformComponent>();
	return transform ? transform->GetWorldPosition() : Math::float3::zero();
}

void CameraComponent::UpdateProjectionMatrix() const
{
	if (m_Type == CameraProjectionType::Perspective)
	{
		m_ProjectionMatrix = Math::float4x4::perspective_lh_zo(m_Fov, m_AspectRatio, m_Near, m_Far);
	}
	else
	{
		float halfWidth = m_OrthoSize * m_AspectRatio * 0.5f;
		float halfHeight = m_OrthoSize * 0.5f;
		m_ProjectionMatrix =
			Math::float4x4::orthographic_lh_zo(halfWidth, halfHeight, m_Near, m_Far);
	}
	m_ProjectionDirty = false;
}

std::unique_ptr<Core::ECS::IComponent> CameraComponent::Clone() const
{
	auto clone = std::make_unique<CameraComponent>();
	clone->m_Type = m_Type;
	clone->m_Fov = m_Fov;
	clone->m_AspectRatio = m_AspectRatio;
	clone->m_Near = m_Near;
	clone->m_Far = m_Far;
	clone->m_OrthoSize = m_OrthoSize;
	clone->m_IsMainCamera = m_IsMainCamera;
	clone->m_ProjectionMatrix = m_ProjectionMatrix;
	clone->m_ProjectionDirty = m_ProjectionDirty;
	return clone;
}
} // namespace Core::ECS::Components
