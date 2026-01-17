///////////////////////////////////////////////////////////////
// Created: 20.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
void CEngine::Start()
{
	Core::CSplashScreen::GetInstance().Show();

	g_bNeedCloseApplication = false;

	SDL_Init(SDL_INIT_EVERYTHING);

	CoreAPI.Initialize();

	CoreAPI.TimeSystem.SetTargetFPS(60);
	CoreAPI.TimeSystem.EnableFPSCap(true);

	LoadRender();

	IMGUI.Initialize();

	LoadGameModule();

	Print("Engine started\n");

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

	Print("Game successfully initialized");
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

	CoreAPI.TimeSystem.Update();

	CoreAPI.Input.BeginFrame();

	IMGUI.OnFrameBegin();

	m_Game->Update();

	Renderer.DrawFrame();

	CoreAPI.Input.EndFrame();
}

void CEngine::Destroy()
{
	// 1. Сначала говорим рендеру забыть про сцену.
	// Это уменьшит счетчик ссылок shared_ptr сцены.
	Renderer.SetCurrentScene(nullptr);
	Renderer.SetCurrentCamera(nullptr);

	// 2. Уничтожаем игру.
	// Если игра держала последний shared_ptr на сцену, то СЦЕНА УДАЛИТСЯ ПРЯМО ЗДЕСЬ.
	// Entity вызовут свои деструкторы, удалят компоненты.
	// Менеджер компонентов в этот момент ЕЩЕ ЖИВ, поэтому всё пройдет гладко.
	if (m_Game)
	{
		m_Game->Shutdown();
		m_Game.reset();
	}

	// 3. Теперь, когда все Entity мертвы, можно безопасно чистить остатки в менеджере.
	Core::ECS::Entity::ReleaseAllComponents();

	// 4. Уничтожаем остальные подсистемы
	IMGUI.Destroy();

	// Можно явно вызвать Shutdown рендера, чтобы освободить окно и бекенд
	Renderer.Shutdown();

	CoreAPI.Destroy();

	SDL_Quit();
}
///////////////////////////////////////////////////////////////
