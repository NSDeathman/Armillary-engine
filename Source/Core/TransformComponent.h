#pragma once
#include "ECSCore.h"
#include <MathAPI/MathAPI.h>
#include <vector>
#include <algorithm>

namespace Core::Components
{
class TransformComponent : public Core::ECS::Component<TransformComponent>
{
  public:
	TransformComponent();
	virtual ~TransformComponent();

	// --- Основные сеттеры (Локальное пространство) ---

	void SetPosition(const Math::float3& position);
	void SetRotation(const Math::quaternion& rotation);
	void SetRotation(const Math::float3& eulerAnglesRadians); // pitch, yaw, roll
	void SetScale(const Math::float3& scale);

	// --- Основные геттеры (Локальное пространство) ---

	const Math::float3& GetLocalPosition() const
	{
		return m_LocalPosition;
	}
	const Math::quaternion& GetLocalRotation() const
	{
		return m_LocalRotation;
	}
	const Math::float3& GetLocalScale() const
	{
		return m_LocalScale;
	}

	// --- Геттеры (Мировое пространство) ---

	Math::float3 GetWorldPosition();
	Math::quaternion GetWorldRotation();
	Math::float3 GetWorldScale(); // Примечание: Lossy scale (приближенный)

	// --- Матрицы ---

	// Возвращает актуальную матрицу локальной трансформации
	const Math::float4x4& GetLocalMatrix();

	// Возвращает актуальную матрицу мировой трансформации (с учетом родителей)
	const Math::float4x4& GetWorldMatrix();

	// --- Управление иерархией ---

	void SetParent(TransformComponent* parent, bool keepWorldTransform = true);
	TransformComponent* GetParent() const
	{
		return m_Parent;
	}
	const std::vector<TransformComponent*>& GetChildren() const
	{
		return m_Children;
	}

	// --- Утилиты перемещения ---

	void Translate(const Math::float3& delta, bool local = false);
	void Rotate(const Math::quaternion& delta);
	void Rotate(const Math::float3& eulerDelta);
	void LookAt(const Math::float3& target, const Math::float3& worldUp = Math::float3::up());

	// --- Векторы направлений (Мировые) ---

	Math::float3 GetForward();
	Math::float3 GetRight();
	Math::float3 GetUp();

	// Реализация клонирования для ECS
	std::unique_ptr<Core::ECS::IComponent> Clone() const override;

  protected:
	// Данные локальной трансформации
	Math::float3 m_LocalPosition;
	Math::quaternion m_LocalRotation;
	Math::float3 m_LocalScale;

	// Кэшированные матрицы
	mutable Math::float4x4 m_LocalMatrix;
	mutable Math::float4x4 m_WorldMatrix;

	// Флаги "грязности"
	mutable bool m_IsLocalDirty = true;
	mutable bool m_IsWorldDirty = true;

	// Иерархия
	TransformComponent* m_Parent = nullptr;
	std::vector<TransformComponent*> m_Children;

  private:
	// Рекурсивно помечает мировые матрицы детей как грязные
	void MarkWorldDirty();

	// Внутренние методы управления списком детей
	void AddChild(TransformComponent* child);
	void RemoveChild(TransformComponent* child);

	// Обновление матриц
	void UpdateLocalMatrix() const;
	void UpdateWorldMatrix() const;
};
} // namespace Core::Components
