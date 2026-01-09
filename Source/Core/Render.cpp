///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Render.h"
#include "Log.h"
#include "imgui_api.h"
#include "Scene.h"
#include "Camera.h"
///////////////////////////////////////////////////////////////
namespace Core
{
Rendeructor RenderBackend;

CRender::CRender()
{
}

CRender::~CRender()
{
	Shutdown();
}

BackendConfig CRender::RenderConfigToBackendConfig(const RenderConfig& config)
{
	BackendConfig Output;

	Output.API = config.API;
	Output.Height = config.Height;
	Output.Width = config.Width;
	Output.ScreenMode = config.ScreenMode;
	Output.WindowHandle = config.WindowHandle;

	return Output;
}

bool CRender::Initialize(const RenderConfig& config)
{
	if (m_Initialized)
	{
		Log("Render already initialized!");
		return true;
	}

	m_Config = config;

	if (config.WindowHandle == nullptr)
	{
		Log("Creating render window...");
		if (!CreateRenderWindow())
		{
			THROW_GRAPHICS("Failed to create rendering window!");
			return false;
		}
		m_Config.WindowHandle = m_Window.GetWindowHandle();
	}

	Log("Initializing render backend...");
	if (!RenderBackend.Create(RenderConfigToBackendConfig(m_Config)))
	{
		THROW_GRAPHICS("Failed to create render backend!");
		return false;
	}

	Log("Initializing render system...");
	Log("  API: %s", m_Config.API == RenderAPI::DirectX11	? "DirectX 11"
					 : m_Config.API == RenderAPI::DirectX12 ? "DirectX 12"
					 : m_Config.API == RenderAPI::OpenGL	? "OpenGL"
					 : m_Config.API == RenderAPI::Vulkan	? "Vulkan"
															: "Unknown");
	Log("  Resolution: %dx%d", m_Config.Width, m_Config.Height);
	Log("  Screen mode: %s", m_Config.ScreenMode == ScreenMode::Fullscreen	 ? "Fullscreen"
							 : m_Config.ScreenMode == ScreenMode::Borderless ? "Borderless"
																			 : "Windowed");

	m_Initialized = true;
	Log("Render system initialized successfully!");

	return true;
}

void CRender::Shutdown()
{
	if (!m_Initialized)
		return;

	Log("Shutting down render system...");

	RenderBackend.Destroy();

	m_Window.Destroy();

	m_Initialized = false;
	Log("Render system shutdown complete.");
}

void CRender::Resize(int width, int height)
{
	if (!m_Initialized || width <= 0 || height <= 0)
		return;

	m_Config.Width = width;
	m_Config.Height = height;

	Log("Resizing render target to %dx%d", width, height);

	RenderBackend.Restart(RenderConfigToBackendConfig(m_Config));

	m_Window.SetResolution(width, height);
}

void CRender::BeginFrame()
{
	if (!m_Initialized)
		return;

	RenderBackend.RenderPassToScreen();

	RenderBackend.Clear(0.39f, 0.58f, 0.93f, 1.0f);
}

void CRender::EndFrame()
{
	if (!m_Initialized)
		return;

	RenderBackend.Present();
}

void CRender::DrawFrame()
{
	BeginFrame();

	if (m_ActiveCamera && m_ActiveScene)
	{
		PipelineState pso;
		pso.Cull = CullMode::None;
		pso.DepthWrite = true;
		pso.DepthFunc = CompareFunc::Less;
		RenderBackend.SetPipelineState(pso);
		m_ActiveScene->Render();
	}

	IMGUI.RenderFrame();

	EndFrame();
}

bool CRender::CreateRenderWindow()
{
	CWindow::Config windowConfig;
	windowConfig.Width = m_Config.Width;
	windowConfig.Height = m_Config.Height;
	windowConfig.Name = m_Config.WindowTitle;

	windowConfig.Mode = (WindowMode)m_Config.ScreenMode; 

	m_Window.Initialize(windowConfig);

	m_Window.Center();

	m_Window.Show();

	return true;
}

void CRender::SetCurrentScene(std::shared_ptr<Core::World::CScene> scene)
{
	m_ActiveScene = scene;
}

void CRender::SetCurrentCamera(std::shared_ptr<Core::World::CCamera> camera)
{
	m_ActiveCamera = camera;
}

} // namespace Core
///////////////////////////////////////////////////////////////
