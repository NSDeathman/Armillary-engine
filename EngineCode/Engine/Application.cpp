///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
#include "Application.h"
#include "splash_screen.h"
#include "render.h"
#include "render_backend.h"
#include "log.h"
#include "cpu_identificator.h"
#include "build_identificator.h"
#include "OptickAPI.h"
#include "filesystem.h"
#include "scene.h"
#include "user_interface.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
bool g_bNeedCloseApplication = false;
SDL_Event g_WindowEvent;
///////////////////////////////////////////////////////////////
CRender* Render = nullptr;
CBackend* RenderBackend = nullptr;
CLog* Log = nullptr;
COptickAPI* OptickAPI = nullptr;
CFilesystem* Filesystem = nullptr;
CScene* Scene = nullptr;
CUserInterface* UserInterface = nullptr;
CMainWindow* MainWindow = nullptr;
///////////////////////////////////////////////////////////////
void CApplication::Start()
{
	Filesystem = new CFilesystem();
	Log = new CLog();

	initializeCPU();

	PrintBuildData();
	
	Msg("Starting Application...");

	MainWindow = new CMainWindow();

	Render = new CRender();
	RenderBackend = new CBackend();

    Render->Initialize();

	UserInterface = new CUserInterface();
	UserInterface->Initialize();

	Scene = new CScene();
}

void CApplication::Destroy()
{
	Scene->Destroy();

	UserInterface->Destroy();
	delete UserInterface;

	Render->Destroy();
	delete Render;

	Filesystem->Destroy();
	delete Filesystem;

	Log->Destroy();
	delete Log;

	delete MainWindow;
}

void RenderFrame()
{
	OPTICK_THREAD("Armillary engine render thread")
	OPTICK_FRAME("RenderThreadTask")
	OPTICK_EVENT("RenderThreadTask")

	Render->OnFrameBegin();

	UserInterface->Render();

	Render->RenderFrame();

	Render->OnFrameEnd();
}

void CApplication::OnFrame()
{
	OPTICK_THREAD("Armillary engine primary thread")
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	UserInterface->OnFrame();

	if (UserInterface->NeedLoadScene())
		Scene->Load();

	if (UserInterface->NeedDestroyScene())
	{
		Scene->Destroy();
		UserInterface->SceneDestroyed();
	}

	RenderFrame();
}

bool m_bKeyPressed = false;

void CApplication::CatchInput()
{
	while (SDL_PollEvent(&g_WindowEvent))
	{
		if (g_WindowEvent.type == SDL_KEYUP)
		{
			if (g_WindowEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				m_bKeyPressed = false;
		}
	}

	const u8* KeyBoardStates = SDL_GetKeyboardState(NULL);
	if (KeyBoardStates[SDL_SCANCODE_ESCAPE] && !m_bKeyPressed)
	{
		Msg("Escape is currently pressed");
		m_bKeyPressed = true;
	}
}

void CApplication::EventLoop()
{
	Msg("Starting event loop...");

	while (!g_bNeedCloseApplication)
	{
		// Handle window events
		if (SDL_PollEvent(&g_WindowEvent))
		{
			if (g_WindowEvent.type == SDL_QUIT)
				break;

			if (g_WindowEvent.type == SDL_KEYUP)
			{
				if (g_WindowEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					m_bKeyPressed = false;
			}

			ImGui_ImplSDL2_ProcessEvent(&g_WindowEvent);
		}

		const u8* KeyBoardStates = SDL_GetKeyboardState(NULL);
		if (KeyBoardStates[SDL_SCANCODE_ESCAPE] && !m_bKeyPressed)
		{
			Msg("Escape is currently pressed");
			m_bKeyPressed = true;
		}

		CatchInput();
		OnFrame();
	}
}   

void CApplication::Process()
{
	SplashScreen = new (CSplashScreen);

	App->Start();

	delete (SplashScreen);

	Msg("Application started successfully");
	Msg("\n");

	App->EventLoop();

	Msg("Destroying Application...");

	App->Destroy();
}
///////////////////////////////////////////////////////////////
