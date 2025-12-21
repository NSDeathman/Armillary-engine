#include "stdafx.h"
#include "Camera.h"
#include "Render.h"

namespace Core::World
{
CCamera::CCamera(): m_Type(ProjectionType::Perspective), 
					m_Fov(Math::Constants::PI / 3.0f), 
					m_AspectRatio(1.777f), 
					m_Near(0.1f), 
					m_Far(1000.0f)
{
	m_Position = Math::float3::zero();
	m_Rotation = Math::quaternion::identity();
	UpdateVectors();
}

CCamera::~CCamera()
{
}

void CCamera::SetPosition(const Math::float3& pos)
{
	m_Position = pos;
}

Math::float3 CCamera::GetPosition() const
{
	return m_Position;
}

void CCamera::SetRotation(const Math::quaternion& rot)
{
	m_Rotation = rot;
	UpdateVectors();
}

Math::quaternion CCamera::GetRotation() const
{
	return m_Rotation;
}

void CCamera::SetPerspective(float fovDeg, float aspect, float zNear, float zFar)
{
	m_Type = ProjectionType::Perspective;
	m_Fov = Math::FastMath::to_radians(fovDeg);
	m_AspectRatio = aspect;
	m_Near = zNear;
	m_Far = zFar;
}

void CCamera::SetOrthographic(float zNear, float zFar)
{
	m_Type = ProjectionType::Orthographic;
	m_Near = zNear;
	m_Far = zFar;
}

void CCamera::SetAspectRatio(float aspect)
{
	m_AspectRatio = aspect;
}

Math::float4x4 CCamera::GetViewMatrix() const
{
	Math::float3 target = m_Position + m_Forward;
	return Math::float4x4::look_at_lh(m_Position, target, m_Up);
}

Math::float4x4 CCamera::GetProjectionMatrix() const
{
	if (m_Type == ProjectionType::Perspective)
	{
		// Рассчитаем ожидаемые масштабы
		float tanHalfFov = tan(m_Fov * 0.5f);
		float yScale = 1.0f / tanHalfFov;
		float xScale = yScale / m_AspectRatio;

		return Math::float4x4::perspective_lh_zo(m_Fov, m_AspectRatio, m_Near, m_Far);
	}
	else
	{
		Math::float2 ScreenRes = Renderer.GetScreenResolution();
		return Math::float4x4::orthographic_lh_zo(ScreenRes.x, ScreenRes.y, m_Near, m_Far);
	}
}

Math::float4x4 CCamera::GetViewProjection() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}

Math::float3 CCamera::GetForward() const
{
	return m_Forward;
}
Math::float3 CCamera::GetRight() const
{
	return m_Right;
}
Math::float3 CCamera::GetUp() const
{
	return m_Up;
}

void CCamera::TranslateLocal(const Math::float3& delta)
{
	m_Position += m_Right * delta.x;
	m_Position += m_Up * delta.y;
	m_Position += m_Forward * delta.z;
}

void CCamera::TranslateGlobal(const Math::float3& delta)
{
	m_Position += delta;
}

void CCamera::UpdateVectors()
{
	m_Forward = m_Rotation.transform_vector(Math::float3::forward()).normalize();
	m_Up = m_Rotation.transform_vector(Math::float3::up()).normalize();
	m_Right = m_Rotation.transform_vector(Math::float3::right()).normalize();
}
} // namespace Core::World
