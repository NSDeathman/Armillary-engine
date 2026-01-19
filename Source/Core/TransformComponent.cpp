#include "stdafx.h"
#include "TransformComponent.h"
#include "ECSCore.h"

namespace Core::ECS::Components
{
using namespace Math;

TransformComponent::TransformComponent()
	: m_LocalPosition(float3::zero()), m_LocalRotation(quaternion::identity()), m_LocalScale(float3::one()),
	  m_LocalMatrix(float4x4::identity()), m_WorldMatrix(float4x4::identity())
{
}

TransformComponent::~TransformComponent()
{
	// При удалении отвязываемся от родителя
	if (m_Parent)
	{
		m_Parent->RemoveChild(this);
	}

	// Отвязываем детей (делаем их сиротами или удаляем - зависит от логики,
	// здесь просто обнуляем родителя у детей)
	for (auto child : m_Children)
	{
		if (child)
			child->m_Parent = nullptr;
	}
}

// ========================================================================
// SETTERS
// ========================================================================

void TransformComponent::SetPosition(const float3& position)
{
	if (m_LocalPosition != position)
	{
		m_LocalPosition = position;
		m_IsLocalDirty = true;
		MarkWorldDirty();
	}
}

void TransformComponent::SetRotation(const quaternion& rotation)
{
	// Проверка на равенство кватернионов может быть дорогой, можно пропустить
	m_LocalRotation = rotation;
	m_IsLocalDirty = true;
	MarkWorldDirty();
}

void TransformComponent::SetRotation(const float3& eulerAnglesRadians)
{
	SetRotation(quaternion::from_euler(eulerAnglesRadians));
}

void TransformComponent::SetScale(const float3& scale)
{
	if (m_LocalScale != scale)
	{
		m_LocalScale = scale;
		m_IsLocalDirty = true;
		MarkWorldDirty();
	}
}

// ========================================================================
// GETTERS (WORLD)
// ========================================================================

float3 TransformComponent::GetWorldPosition()
{
	return GetWorldMatrix().get_translation();
}

quaternion TransformComponent::GetWorldRotation()
{
	return GetWorldMatrix().get_rotation();
}

float3 TransformComponent::GetWorldScale()
{
	return GetWorldMatrix().get_scale();
}

const float4x4& TransformComponent::GetLocalMatrix()
{
	if (m_IsLocalDirty)
	{
		UpdateLocalMatrix();
	}
	return m_LocalMatrix;
}

const float4x4& TransformComponent::GetWorldMatrix()
{
	if (m_IsWorldDirty)
	{
		UpdateWorldMatrix();
	}
	return m_WorldMatrix;
}

// ========================================================================
// HIERARCHY
// ========================================================================

void TransformComponent::SetParent(TransformComponent* parent, bool keepWorldTransform)
{
	if (m_Parent == parent)
		return;

	// 1. Сохраняем текущее мировое состояние, если нужно
	float3 oldPos, oldScale;
	quaternion oldRot;

	if (keepWorldTransform)
	{
		oldPos = GetWorldPosition();
		oldRot = GetWorldRotation();
		oldScale = GetWorldScale();
	}

	// 2. Отвязываемся от старого родителя
	if (m_Parent)
	{
		m_Parent->RemoveChild(this);
	}

	// 3. Привязываемся к новому
	m_Parent = parent;
	if (m_Parent)
	{
		m_Parent->AddChild(this);
	}

	// 4. Пересчитываем локальные координаты, чтобы сохранить мировые
	if (keepWorldTransform)
	{
		if (m_Parent)
		{
			float4x4 parentInv = m_Parent->GetWorldMatrix().inverted();
			float4x4 newLocal = parentInv * float4x4::TRS(oldPos, oldRot, oldScale);

			m_LocalPosition = newLocal.get_translation();
			m_LocalRotation = newLocal.get_rotation();
			m_LocalScale = newLocal.get_scale();
		}
		else
		{
			m_LocalPosition = oldPos;
			m_LocalRotation = oldRot;
			m_LocalScale = oldScale;
		}
	}

	m_IsLocalDirty =
		true; // На всякий случай обновляем локальную матрицу (хотя если keepWorldTransform=true, она могла измениться)
	MarkWorldDirty();
}

void TransformComponent::AddChild(TransformComponent* child)
{
	m_Children.push_back(child);
}

void TransformComponent::RemoveChild(TransformComponent* child)
{
	auto it = std::remove(m_Children.begin(), m_Children.end(), child);
	if (it != m_Children.end())
	{
		m_Children.erase(it, m_Children.end());
	}
}

// ========================================================================
// UTILS
// ========================================================================

void TransformComponent::Translate(const float3& delta, bool local)
{
	if (local)
	{
		// Движение вдоль собственных осей
		m_LocalPosition += m_LocalRotation.transform_vector(delta);
	}
	else
	{
		// Движение в глобальных осях
		// Если есть родитель, нужно перевести глобальную дельту в локальное пространство родителя
		if (m_Parent)
		{
			// Это сложнее: нужно инвертировать поворот родителя
			quaternion parentRotInv = m_Parent->GetWorldRotation().inverse();
			m_LocalPosition += parentRotInv.transform_vector(delta);
		}
		else
		{
			m_LocalPosition += delta;
		}
	}
	m_IsLocalDirty = true;
	MarkWorldDirty();
}

void TransformComponent::Rotate(const quaternion& delta)
{
	m_LocalRotation = m_LocalRotation * delta;
	m_IsLocalDirty = true;
	MarkWorldDirty();
}

void TransformComponent::Rotate(const float3& eulerDelta)
{
	Rotate(quaternion::from_euler(eulerDelta));
}

void TransformComponent::LookAt(const float3& target, const float3& worldUp)
{
	// Получаем мировые координаты
	float3 eye = GetWorldPosition();

	// Используем MathAPI для создания матрицы взгляда или кватерниона
	// LookAt матрица обычно View Matrix (инвертированная), нам нужна World Transform
	// Поэтому Z смотрит ОТ цели в View matrix, но объект должен смотреть НА цель.
	// В твоей MathAPI float4x4::look_at создает View матрицу.

	float3 forward = (target - eye).normalize();

	// Простая реализация через кватернион (предполагаем, что forward это Z+)
	// Если нужно, чтобы Z+ смотрел на цель:
	m_LocalRotation = quaternion::look_rotation(forward, worldUp);

	// Если есть родитель, нужно компенсировать его вращение
	if (m_Parent)
	{
		m_LocalRotation = m_Parent->GetWorldRotation().inverse() * m_LocalRotation;
	}

	m_IsLocalDirty = true;
	MarkWorldDirty();
}

Math::float3 TransformComponent::GetForward()
{
    const Math::float4x4& world = GetWorldMatrix();
    Math::quaternion rotation = GetWorldRotation();
    return rotation.transform_vector(Math::float3(0, 0, 1)); // Z-forward
}

Math::float3 TransformComponent::GetRight()
{
    Math::quaternion rotation = GetWorldRotation();
    return rotation.transform_vector(Math::float3(1, 0, 0)); // X-right
}

Math::float3 TransformComponent::GetUp()
{
    Math::quaternion rotation = GetWorldRotation();
    return rotation.transform_vector(Math::float3(0, 1, 0)); // Y-up
}

std::unique_ptr<Core::ECS::IComponent> TransformComponent::Clone() const
{
	auto clone = std::make_unique<TransformComponent>();
	clone->m_LocalPosition = m_LocalPosition;
	clone->m_LocalRotation = m_LocalRotation;
	clone->m_LocalScale = m_LocalScale;
	// Детей не клонируем автоматически в этом методе, это ответственность Entity::Clone
	return clone;
}

// ========================================================================
// INTERNALS
// ========================================================================

void TransformComponent::MarkWorldDirty()
{
	if (m_IsWorldDirty)
		return; // Уже помечено, не нужно идти глубже, дети уже помечены

	m_IsWorldDirty = true;

	for (auto child : m_Children)
	{
		child->MarkWorldDirty();
	}
}

void TransformComponent::UpdateLocalMatrix() const
{
	m_LocalMatrix = float4x4::TRS(m_LocalPosition, m_LocalRotation, m_LocalScale);
	m_IsLocalDirty = false;
}

void TransformComponent::UpdateWorldMatrix() const
{
	// Сначала обновляем локальную, если нужно
	if (m_IsLocalDirty)
		UpdateLocalMatrix();

	if (m_Parent)
	{
		// Рекурсивно запрашиваем матрицу родителя (она обновится, если грязная)
		m_WorldMatrix = m_Parent->GetWorldMatrix() * m_LocalMatrix;
	}
	else
	{
		m_WorldMatrix = m_LocalMatrix;
	}

	m_IsWorldDirty = false;
}
} // namespace Core::Components
