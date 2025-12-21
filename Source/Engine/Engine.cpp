///////////////////////////////////////////////////////////////
// Created: 20.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
///////////////////////////////////////////////////////////////
void CEngine::Start()
{
	Core::CSplashScreen::GetInstance().Show();

	g_bNeedCloseApplication = false;

	SDL_Init(SDL_INIT_EVERYTHING);

	LogInit(std::string(LOGS));

	Log("\n");
	Log("Starting engine\n");

	FS_INIT();

	SAFE_EXECUTE_LOG(PrintBuildData(), "Printing build data");

	InitializeCPU();

	ErrorHandler::initialize();

	ASYNC_API_INIT();

	TIME_API.Initialize();
	TIME_API.SetTargetFPS(60);
	TIME_API.EnableFPSCap(true);

	INPUT.Initialize();

	LoadRender();

	IMGUI.Initialize();

	LoadGameModule();

	Log("Engine started\n");

	Core::CSplashScreen::GetInstance().Hide();
}

bool CEngine::LoadGameModule()
{
	m_Game.reset(CreateGame());

	if (!m_Game)
	{
		THROW_ENGINE("Failed to create game instance");
		return false;
	}

	if (!m_Game->Initialize())
	{
		THROW_ENGINE("Game initialization failed");
		return false;
	}

	Log("Game successfully initialized");
	return true;
}

void CEngine::LoadRender()
{
	RenderConfig Config;
	Config.API = RenderAPI::DirectX11;
	Config.Height = 720;
	Config.Width = 1280;
	Config.ScreenMode = ScreenMode::Windowed;
	Renderer.Initialize(Config);
}

void CEngine::Process()
{
	Start();

	while (!g_bNeedCloseApplication)
		Update();

	Destroy();
}

void CEngine::HandleSDLEvents()
{
	SDL_Event SDLEvent;
	while (SDL_PollEvent(&SDLEvent))
	{
		IMGUI.ProcessEvent(&SDLEvent);

		if (SDLEvent.type == SDL_QUIT)
			g_bNeedCloseApplication = true;
	}
}

void CEngine::Update()
{
	HandleSDLEvents();

	TIME_API.Update();

	INPUT.BeginFrame();

	IMGUI.OnFrameBegin();

	m_Game->Update();

	Renderer.DrawFrame();

	INPUT.EndFrame();
}

void CEngine::Destroy()
{
	m_Game->Shutdown();

	IMGUI.Destroy();

	LogDestroy();

	ASYNC_API_DESTROY();

	FS_DESTROY();

	SDL_Quit();
}
///////////////////////////////////////////////////////////////
