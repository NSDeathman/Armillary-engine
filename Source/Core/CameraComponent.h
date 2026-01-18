#pragma once

#include "ECSCore.h"
#include "Camera.h"
#include "TransformComponent.h"

class CameraComponent : public Core::ECS::Component<CameraComponent>
{
  public:
	CameraComponent() = default;

	// Доступ к внутренней камере для настройки (FOV и т.д.)
	Core::World::CCamera& GetCamera()
	{
		return m_Camera;
	}
	const Core::World::CCamera& GetCamera() const
	{
		return m_Camera;
	}

	// Основной метод обновления: синхронизирует ECS Transform с камерой
	void OnUpdate(float dt) override
	{
		if (m_Owner)
		{
			auto* transform = m_Owner->Get<Core::ECS::Components::TransformComponent>();
			if (transform)
			{
				// Синхронизируем позицию и поворот
				m_Camera.SetPosition(transform->GetWorldPosition());
				m_Camera.SetRotation(transform->GetWorldRotation());
			}
		}
	}

	std::unique_ptr<Core::ECS::IComponent> Clone() const override
	{
		auto clone = std::make_unique<CameraComponent>();
		clone->m_Camera = m_Camera; // CCamera должна поддерживать копирование
		return clone;
	}

  private:
	Core::World::CCamera m_Camera;
};