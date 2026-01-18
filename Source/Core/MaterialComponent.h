#pragma once
#include "ECSCore.h"
#include "TransformComponent.h"
#include <Rendeructor/Rendeructor.h>

namespace Core::ECS::Components
{

class MaterialComponent : public Core::ECS::Component<MaterialComponent>
{
  public:
	ShaderPass Pass; // Структура из Rendeructor

	// Можно добавить свойства материала (цвет, текстуры и т.д.)
	// Пока используем то, что есть в ShaderPass и Rendeructor

	std::unique_ptr<Core::ECS::IComponent> Clone() const override
	{
		auto clone = std::make_unique<MaterialComponent>();
		clone->Pass = Pass;
		return clone;
	}
};

} // namespace Core::ECS::Components
