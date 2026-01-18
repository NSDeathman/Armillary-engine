#include "stdafx.h"
#include "Game.h"
#include <ECSCore.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <MaterialComponent.h>

using namespace Math;
using namespace Core;
using namespace Core::World;
using namespace Core::ECS;
using namespace Core::ECS::Components;

// Указатель на сущность куба для доступа в Update (для вращения или GUI)
Entity* m_CubeEntity = nullptr;
// Указатель на сущность камеры
Entity* m_CameraEntity = nullptr;

IGame* CreateGame()
{
	return new CGame();
}

bool CGame::Initialize()
{
	// 1. Создаем сцену
	m_Scene = std::make_shared<CScene>();
	if (!m_Scene->Initialize())
		return false;

	// 2. Компилируем шейдер
	ShaderPass standardShader;
	std::string shaderPath = CoreAPI.Filesystem.GetGameResourcesPath({"shaders", "standard.shader"}).string();
	if (!CoreAPI.Filesystem.FileExists(shaderPath))
	{
		PrintError("Shader missing: %s", shaderPath.c_str());
		return false;
	}
	standardShader.VertexShaderPath = shaderPath;
	standardShader.VertexShaderEntryPoint = "VS";
	standardShader.PixelShaderPath = shaderPath;
	standardShader.PixelShaderEntryPoint = "PS";
	RenderBackend.CompilePass(standardShader);

	// 3. Создаем КАМЕРУ
	m_CameraEntity = m_Scene->CreateEntity("MainCamera");

	// --- ИСПРАВЛЕНИЕ 1: Добавляем и получаем компоненты раздельно ---
	m_CameraEntity->Add<TransformComponent>();
	auto* camTrans = m_CameraEntity->Get<TransformComponent>();
	camTrans->SetPosition(float3(0.0f, 0.0f, -5.0f));

	m_CameraEntity->Add<CameraComponent>();
	auto* camComp = m_CameraEntity->Get<CameraComponent>();

	RenderConfig cfg = Renderer.GetConfig();
	float aspect = (float)cfg.Width / (float)cfg.Height;
	camComp->GetCamera().SetPerspective(60.0f, aspect, 0.01f, 1000.0f);

	m_Scene->SetMainCameraEntity(m_CameraEntity);

	// 4. Создаем КУБ
	m_CubeEntity = m_Scene->CreateEntity("TestCube");

	// --- ИСПРАВЛЕНИЕ 2: Используем GetOrAdd для краткости ---
	// Transform
	auto* cubeTrans = m_CubeEntity->GetOrAdd<TransformComponent>();
	cubeTrans->SetPosition(float3(0.0f, 0.0f, 0.0f));

	// Mesh
	auto* meshComp = m_CubeEntity->GetOrAdd<MeshComponent>();
	auto cubeMesh = std::make_shared<Mesh>();
	Mesh::GenerateCube(*cubeMesh, 1.0f);
	meshComp->SetMesh(cubeMesh);

	// Material
	auto* matComp = m_CubeEntity->GetOrAdd<MaterialComponent>();
	matComp->Pass = standardShader;

	// 5. Контроллер
	auto flyController = std::make_unique<CFlyingCameraController>();
	flyController->MoveSpeed = 5.0f;
	m_CameraController = std::move(flyController);

	Renderer.SetCurrentScene(m_Scene);

	return true;
}

void CGame::Shutdown()
{
	m_Scene.reset();
}

void CGame::Update()
{
	float dt = CoreAPI.TimeSystem.GetDeltaTime();

	// 1. Логика камеры
	bool isRMBDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

	if (isRMBDown && m_CameraEntity)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);

		// ВАЖНО: Контроллер хочет CCamera&, но в ECS мы должны двигать Transform!
		// Вариант А: Контроллер двигает CCamera внутри компонента, а мы должны
		// синхронизировать Transform ОБРАТНО. Это костыль.
		// Вариант Б (Правильный): Переписать Controller, чтобы он принимал TransformComponent.

		// Временное решение (Вариант А с синхронизацией):
		auto* camComp = m_CameraEntity->Get<CameraComponent>();
		auto* camTrans = m_CameraEntity->Get<TransformComponent>();

		if (camComp && camTrans)
		{
			// Контроллер обновляет внутреннее состояние CCamera
			m_CameraController->Update(camComp->GetCamera(), dt);

			// Синхронизируем Transform ECS с тем, что насчитал контроллер
			// (Потому что в ECS позиция хранится в Transform)
			camTrans->SetPosition(camComp->GetCamera().GetPosition());
			camTrans->SetRotation(camComp->GetCamera().GetRotation());
		}
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	// Вращение куба (пример логики)
	if (m_CubeEntity)
	{
		auto* t = m_CubeEntity->Get<TransformComponent>();
		t->Rotate(Math::float3(0, dt * 0.5f, 0)); // Крутим по Y
	}

	// 2. Update сцены (обновляет Transform matrices и т.д.)
	m_Scene->Update(dt);

	// 2. Интерфейс
	ImGui::Begin("Object Editor");

	ImGui::End();
}
