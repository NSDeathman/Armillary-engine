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

	LogInit(LOGS);

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
		std::cerr << "Failed to find CreateGame function in Game.dll" << std::endl;
		FreeLibrary(m_GameModule);
		m_GameModule = nullptr;
		return false;
	}

	// 3. Создаем экземпляр игры
	m_Game.reset(m_createGameFn());
	if (!m_Game)
	{
		std::cerr << "Failed to create game instance" << std::endl;
		FreeLibrary(m_GameModule);
		m_GameModule = nullptr;
		return false;
	}

	// 4. Инициализируем игру
	if (!m_Game->Initialize())
	{
		std::cerr << "Game initialization failed" << std::endl;
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
	CRender::Config config;

	config.Width = 1280;
	config.Height = 720;
	config.GraphicsAPI = GraphicsAPI::DirectX9;

	//CRITICAL_ERROR("Can't load render");

	//bool Result = m_Render.Initialize(config);
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
	if (SDL_PollEvent(&SDLEvent)) [[likely]]
	{
		if (SDLEvent.type == SDL_QUIT) [[unlikely]]
			g_bNeedCloseApplication = true;

		//ImGui_ImplSDL2_ProcessEvent(&SDLEvent);
	}
}

void CEngine::Update()
{
	HandleSDLEvents();

	TIME_API.Update();

	INPUT.Update();

	m_Game->Update();

	//m_Render.BeginFrame();

	m_Game->Render();

	//m_Render.EndFrame();
	//m_Render.Present();
}

void CEngine::Destroy()
{
	m_Game->Shutdown();

	LogDestroy();

	ASYNC_API_DESTROY();

	FS_DESTROY();

	SDL_Quit();
}
///////////////////////////////////////////////////////////////
