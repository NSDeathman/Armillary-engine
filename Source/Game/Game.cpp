#include "stdafx.h"
#include "Game.h"
#include "../Core/StaticModel.h"
#include "../Core/StaticObject.h"
#include <MathAPI/MathAPI.h>

using namespace Core;
using namespace Core::World;
using namespace Math;

std::shared_ptr<Core::World::CStaticObject> m_TestCube;

IGame* CreateGame()
{
	return new CGame();
}

bool CGame::Initialize()
{
	Core::InitializeImGuiFromCore();

	m_Scene = std::make_shared<CScene>();

	if (!m_Scene->Initialize())
		return false;

	m_MainCamera = std::make_shared<CCamera>();
	m_MainCamera->SetPosition(float3(0.0f, 0.0f, -20.0f));

	RenderConfig cfg = Renderer.GetConfig();
	float aspect = (float)cfg.Width / (float)cfg.Height;
	m_MainCamera->SetPerspective(67.5f, aspect, 0.01f, 100.0f);

	m_MainCamera->SetRotation(Math::quaternion::identity());

	Renderer.SetCurrentCamera(m_MainCamera);

	auto flyController = std::make_unique<CFlyingCameraController>();
	flyController->MoveSpeed = 10.0f;
	m_CameraController = std::move(flyController);

	m_CubeModel = std::make_shared<StaticModel>();
	Mesh::GenerateCube(*m_CubeModel->MeshData, 0.5f);

	uint32_t whitePixel = 0xFFFFFFFF;
	m_CubeModel->Albedo->Create(1, 1, TextureFormat::RGBA8, &whitePixel);

	auto cubeObj = std::make_shared<CStaticObject>("Cube", m_CubeModel);
	cubeObj->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	m_TestCube = cubeObj;

	ShaderPass standardShader;
	std::string shaderPath = FS.GetGameResourcesPath({"shaders", "standard.shader"}).string();

	if (!FS.FileExists(shaderPath))
	{
		ErrLog("Shader missing: %s", shaderPath.c_str());
		return false;
	}

	standardShader.VertexShaderPath = shaderPath;
	standardShader.VertexShaderEntryPoint = "VS";
	standardShader.PixelShaderPath = shaderPath;
	standardShader.PixelShaderEntryPoint = "PS";

	RenderBackend.CompilePass(standardShader);

	cubeObj->SetShader(standardShader);

	m_Scene->AddObject(cubeObj);

	Renderer.SetCurrentScene(m_Scene);

	return true;
}

void CGame::Shutdown()
{
	m_Scene.reset();
}

void CGame::Update()
{
	float dt = TIME_API.GetDeltaTime();

	// 1. Логика камеры и курсора
	bool isRMBDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

	if (isRMBDown)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		if (m_CameraController)
			m_CameraController->Update(*m_MainCamera, dt);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	m_Scene->Update(dt);

	// 2. Интерфейс
	ImGui::Begin("Object Editor");

	if (m_TestCube)
	{
		auto& transform = m_TestCube->GetTransform();

		// A. Получаем матрицы движка (все в LH системе)
		Math::float4x4 view = m_MainCamera->GetViewMatrix();
		Math::float4x4 projection = m_MainCamera->GetProjectionMatrix();

		// Получаем мировую матрицу напрямую из Transform
		const Math::float4x4& world = transform.GetMatrix();

		// B. Подготовка массивов для ImGuizmo с преобразованием LH->RH
		float viewM[16], projM[16], worldM[16];

		RenderConfig cfg = Renderer.GetConfig();
		float aspect = (float)cfg.Width / (float)cfg.Height;
	}

	ImGui::End();
}
