///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
// Application and stdafx
#include "Application.h"

// Main engine parts
#include "render.h"
#include "render_backend.h"
#include "log.h"
#include "Input.h"
#include "filesystem.h"
#include "scene.h"

// Windows
#include "splash_screen.h"
#include "main_window.h"

// Helpers
#include "cpu_identificator.h"
#include "build_identificator.h"

// Optick
#include "OptickAPI.h"

// UI
#include "user_interface.h"

// Threading
#include "threading.h"
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
CInput* Input = nullptr;
CScheduler* Scheduler = nullptr;
///////////////////////////////////////////////////////////////
void CApplication::Start()
{
	Filesystem = new CFilesystem();
	Log = new CLog();

	initializeCPU();

	PrintBuildData();
	
	Msg("Starting Application...");

	Scheduler = new CScheduler();

	Input = new CInput();

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

	delete Input;

	delete MainWindow;

	delete Scheduler;
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

void CApplication::HandleSDLEvents()
{
	// Handle window events
	if (SDL_PollEvent(&g_WindowEvent))
	{
		if (g_WindowEvent.type == SDL_QUIT)
			g_bNeedCloseApplication = true;

		ImGui_ImplSDL2_ProcessEvent(&g_WindowEvent);
	}
}

void InputUpdateTask()
{
	Input->OnFrame();
}

void CApplication::OnFrame()
{
	OPTICK_THREAD("Armillary engine primary thread")
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	HandleSDLEvents();

	Scheduler->Add(InputUpdateTask);

	if (Input->KeyPressed(SDL_SCANCODE_ESCAPE))
		Msg("Escape pressed");

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

void CApplication::EventLoop()
{
	Msg("Starting event loop...");

	while (!g_bNeedCloseApplication)
	{
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
