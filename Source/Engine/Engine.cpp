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
	Log("Loading game module");

	const std::string& dllPath = "Game.dll";
	CreateGameFn m_createGameFn = nullptr;

	// 1. Загружаем DLL в память
	m_GameModule = LoadLibraryA(dllPath.c_str());
	if (!m_GameModule)
	{
		std::cerr << "Failed to load Game.dll from: " << dllPath << std::endl;
		return false;
	}

	// 2. Получаем указатель на фабричную функцию
	m_createGameFn = (CreateGameFn)GetProcAddress(m_GameModule, "CreateGame");
	if (!m_createGameFn)
	{
		THROW_ENGINE("Failed to find CreateGame function in Game.dll");
		FreeLibrary(m_GameModule);
		m_GameModule = nullptr;
		return false;
	}

	// 3. Создаем экземпляр игры
	m_Game.reset(m_createGameFn());
	if (!m_Game)
	{
		THROW_ENGINE("Failed to create game instance");
		FreeLibrary(m_GameModule);
		m_GameModule = nullptr;
		return false;
	}

	// 4. Инициализируем игру
	if (!m_Game->Initialize())
	{
		THROW_ENGINE("Game initialization failed");
		FreeLibrary(m_GameModule);
		m_Game.reset();
		m_GameModule = nullptr;
		return false;
	}

	Log("Game successfully loaded");

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

	INPUT.Update();

	IMGUI.OnFrameBegin();

	m_Game->Update();

	Renderer.DrawFrame();
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
