#include "stdafx.h"
#include "Scene.h"
#include "Components.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "Render.h"

namespace Core::World
{
using namespace Core::Components;
using namespace Core::ECS;

CScene::CScene()
{
}
CScene::~CScene()
{
	m_Entities.clear();
}

bool CScene::Initialize()
{
	return true;
}

void CScene::Update(float dt)
{
	// Обновляем все сущности
	// В будущем здесь можно разделить логику на системы (PhysicsSystem, ScriptSystem и т.д.)
	for (auto& entity : m_Entities)
	{
		if (entity->IsActive())
			entity->Update(dt);
	}

	// Удаление помеченных на уничтожение сущностей можно сделать здесь (Cleanup)
}

void CScene::Render()
{
	// 1. Находим камеру
	if (!m_MainCameraEntity || !m_MainCameraEntity->IsActive())
		return;

	auto* cameraComp = m_MainCameraEntity->Get<CameraComponent>();
	if (!cameraComp)
		return;

	auto& camera = cameraComp->GetCamera();

	// 2. Настраиваем глобальные константы кадра (View, Projection)
	// Структура констант кадра (должна соответствовать шейдеру, cbuffer FrameBuffer : register(b0))
	struct FrameConstants
	{
		Math::float4x4 View;
		Math::float4x4 Projection;
		Math::float3 CameraPos;
		float Padding;
	} frameData;

	frameData.View = camera.GetViewMatrix();
	frameData.Projection = camera.GetProjectionMatrix();
	frameData.CameraPos = camera.GetPosition();

	RenderBackend.SetCustomConstant("FrameBuffer", frameData);

	// 3. Итерируемся по всем сущностям, которые можно нарисовать
	// (Требуется: Mesh + Transform + Material)
	for (auto& entity : m_Entities)
	{
		if (!entity->IsActive())
			continue;

		auto* meshComp = entity->Get<MeshComponent>();
		auto* transComp = entity->Get<TransformComponent>();
		auto* matComp = entity->Get<MaterialComponent>();

		if (meshComp && transComp && matComp)
		{
			auto mesh = meshComp->GetMesh();
			if (!mesh)
				continue;

			// 3.1 Устанавливаем пайплайн (шейдеры)
			RenderBackend.SetShaderPass(matComp->Pass);

			// 3.2 Устанавливаем константы объекта (World Matrix)
			// cbuffer ObjectBuffer : register(b1)
			struct ObjectConstants
			{
				Math::float4x4 World;
			} objData;

			objData.World = transComp->GetWorldMatrix();
			RenderBackend.SetCustomConstant("ObjectBuffer", objData);

			// 3.3 Рисуем
			RenderBackend.DrawMesh(*mesh);
		}
	}
}

Core::ECS::Entity* CScene::CreateEntity(const std::string& name)
{
	auto entity = Core::ECS::CreateEntity(name);
	Core::ECS::Entity* ptr = entity.get();
	m_Entities.push_back(std::move(entity));
	return ptr;
}

void CScene::RemoveEntity(Core::ECS::Entity* entity)
{
	// Простая реализация удаления (медленная O(N), для продакшена нужен ID map)
	std::erase_if(m_Entities, [&](const auto& e) { return e.get() == entity; });
}

Core::ECS::Entity* CScene::GetMainCameraEntity() const
{
	return m_MainCameraEntity;
}

void CScene::SetMainCameraEntity(Core::ECS::Entity* entity)
{
	m_MainCameraEntity = entity;
}
} // namespace Core::World
