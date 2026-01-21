#include "stdafx.h"
#include "Game.h"
#include <ECSCore.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <FlyingCameraControllerComponent.h>
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

	 // 3. Создаем КАМЕРУ (полностью в ECS)
	m_CameraEntity = m_Scene->CreateEntity("MainCamera");

	// Transform
	auto* camTrans = m_CameraEntity->GetOrAdd<TransformComponent>();
	camTrans->SetPosition(Math::float3(0.0f, 0.0f, -5.0f));

	// Camera
	auto* camComp = m_CameraEntity->GetOrAdd<CameraComponent>();
	RenderConfig cfg = Renderer.GetConfig();
	float aspect = (float)cfg.Width / (float)cfg.Height;
	camComp->SetPerspective(60.0f, aspect, 0.01f, 1000.0f);
	camComp->SetMainCamera(true);

	// Controller
	auto* controller = m_CameraEntity->GetOrAdd<FlyingCameraControllerComponent>();
	controller->MoveSpeed = 5.0f;
	controller->MouseSensitivity = 0.5f;

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

	// Управление мышью теперь полностью в компоненте FlyingCameraController
	bool isRMBDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	if (!isRMBDown)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	// Вращение куба
	if (m_CubeEntity)
	{
		auto* t = m_CubeEntity->Get<TransformComponent>();
		if (t)
			t->Rotate(Math::float3(0, dt * 0.5f, 0));
	}

	// Обновление сцены (автоматически обновит все компоненты, включая FlyingCameraController)
	m_Scene->Update(dt);
}
