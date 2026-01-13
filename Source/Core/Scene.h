#pragma once
#include <vector>
#include <memory>
#include "ECSCore.h"

namespace Core::World
{
class CScene
{
  public:
	CScene();
	~CScene();

	bool Initialize();
	void Update(float dt);

	// Метод отрисовки, который будет вызывать CRender
	void Render();

	// Управление сущностями
	Core::ECS::Entity* CreateEntity(const std::string& name);
	void RemoveEntity(Core::ECS::Entity* entity);

	// Поиск главной камеры (для рендера)
	Core::ECS::Entity* GetMainCameraEntity() const;
	void SetMainCameraEntity(Core::ECS::Entity* entity);

  private:
	std::vector<std::unique_ptr<Core::ECS::Entity>> m_Entities;

	// Кэшированный указатель на камеру (чтобы не искать каждый кадр)
	Core::ECS::Entity* m_MainCameraEntity = nullptr;
};
} // namespace Core::World
