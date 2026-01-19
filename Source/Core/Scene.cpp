#include "stdafx.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "Render.h"

namespace Core::World
{
using namespace Core::ECS;
using namespace Core::ECS::Components;

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
	// 1. Находим главную камеру
	Entity* mainCamera = GetMainCameraEntity();
	if (!mainCamera || !mainCamera->IsActive())
		return;

	auto* cameraComp = mainCamera->Get<CameraComponent>();
	auto* transformComp = mainCamera->Get<TransformComponent>();

	if (!cameraComp || !transformComp)
		return;

	// 2. Отправка Frame Buffer
	struct FrameConstants
	{
		Math::float4x4 View;
		Math::float4x4 Projection;
		Math::float4x4 ViewProjection;
		Math::float3 CameraPos;
		float Padding;
	} frameData;

	frameData.View = cameraComp->GetViewMatrix();
	frameData.Projection = cameraComp->GetProjectionMatrix();
	frameData.ViewProjection = frameData.View * frameData.Projection;
	frameData.CameraPos = transformComp->GetWorldPosition();
	frameData.Padding = 0.0f;

	RenderBackend.SetCustomConstant("FrameBuffer", frameData);

	// 3. РИСУЕМ ОБЪЕКТЫ
	for (auto& entity : m_Entities)
	{
		if (!entity->IsActive())
			continue;

		auto* meshComp = entity->Get<MeshComponent>();
		auto* transComp = entity->Get<TransformComponent>();
		auto* matComp = entity->Get<MaterialComponent>(); // <-- Материал должен использовать simple.shader

		if (meshComp && transComp && matComp)
		{
			auto mesh = meshComp->GetMesh();
			if (!mesh)
				continue;

			// Устанавливаем шейдер
			RenderBackend.SetShaderPass(matComp->Pass);

			// 4. ОТПРАВКА OBJECT BUFFER (b1)
			struct ObjectConstants
			{
				Math::float4x4 World;
			} objData;

			// НЕ транспонируем
			objData.World = transComp->GetWorldMatrix();

			// Имя должно совпадать с cbuffer ObjectBuffer в шейдере
			RenderBackend.SetCustomConstant("ObjectBuffer", objData);

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
