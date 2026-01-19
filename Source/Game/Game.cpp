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

void TestQuaternionMath()
{
	Print("\n=== Quaternion Math Test ===");

	// Тест 1: 45 градусов вокруг Y
	{
		Math::quaternion q1 = Math::quaternion::rotation_y(Math::FastMath::to_radians(45.0f));
		Math::float3 euler1 = q1.to_euler();
		Print("Test 1 - 45° Y rotation:");
		Print("  Quaternion: (%.3f, %.3f, %.3f, %.3f)", q1.x, q1.y, q1.z, q1.w);
		Print("  Euler Angles: X=%.2f°, Y=%.2f°, Z=%.2f°", Math::FastMath::to_degrees(euler1.x),
			  Math::FastMath::to_degrees(euler1.y), Math::FastMath::to_degrees(euler1.z));

		// Проверяем преобразование вектора
		Math::float3 forward = q1.transform_vector(Math::float3(0, 0, 1));
		Print("  Forward after 45° Y: (%.3f, %.3f, %.3f) - expected (0.707, 0, 0.707)", forward.x, forward.y,
			  forward.z);
	}

	// Тест 2: from_euler
	{
		float pitch = Math::FastMath::to_radians(30.0f);
		float yaw = Math::FastMath::to_radians(45.0f);
		float roll = 0.0f;

		Math::quaternion q2 = Math::quaternion::from_euler(pitch, yaw, roll);
		Math::float3 euler2 = q2.to_euler();
		Print("\nTest 2 - from_euler(30°, 45°, 0°):");
		Print("  Quaternion: (%.3f, %.3f, %.3f, %.3f)", q2.x, q2.y, q2.z, q2.w);
		Print("  Euler Angles: X=%.2f°, Y=%.2f°, Z=%.2f°", Math::FastMath::to_degrees(euler2.x),
			  Math::FastMath::to_degrees(euler2.y), Math::FastMath::to_degrees(euler2.z));
	}

	// Тест 3: Порядок умножения
	{
		Math::quaternion qY = Math::quaternion::rotation_y(Math::FastMath::to_radians(45.0f));
		Math::quaternion qX = Math::quaternion::rotation_x(Math::FastMath::to_radians(30.0f));

		// Способ 1: Yaw then Pitch
		Math::quaternion q3a = qY * qX;
		Math::float3 euler3a = q3a.to_euler();

		// Способ 2: Pitch then Yaw
		Math::quaternion q3b = qX * qY;
		Math::float3 euler3b = q3b.to_euler();

		Print("\nTest 3 - Multiplication order:");
		Print("  qY * qX (Yaw then Pitch): X=%.2f°, Y=%.2f°, Z=%.2f°", Math::FastMath::to_degrees(euler3a.x),
			  Math::FastMath::to_degrees(euler3a.y), Math::FastMath::to_degrees(euler3a.z));
		Print("  qX * qY (Pitch then Yaw): X=%.2f°, Y=%.2f°, Z=%.2f°", Math::FastMath::to_degrees(euler3b.x),
			  Math::FastMath::to_degrees(euler3b.y), Math::FastMath::to_degrees(euler3b.z));
	}
}

bool CGame::Initialize()
{
	TestQuaternionMath();

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
