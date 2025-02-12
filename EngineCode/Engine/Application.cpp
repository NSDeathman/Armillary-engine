///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
// Application and stdafx
#include "Application.h"

// Main engine parts
#ifndef USE_DX11
#include "render_DX9.h"
#include "render_backend_DX9.h"
#else
#include "render_DX11.h"
//#include "render_backend_DX11.h"
#endif

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

// Camera
#include "camera.h"
///////////////////////////////////////////////////////////////
uint16_t g_ScreenWidth = 720;
uint16_t g_ScreenHeight = 480;
///////////////////////////////////////////////////////////////
bool g_bNeedCloseApplication = false;
SDL_Event g_WindowEvent;
///////////////////////////////////////////////////////////////
#ifdef USE_DX11
CRenderDX11* Render = nullptr;
//CRenderBackendDX11* RenderBackend = nullptr;
#else
CRenderDX9* Render = nullptr;
CRenderBackendDX9* RenderBackend = nullptr;
#endif

CLog* Log = nullptr;
COptickAPI* OptickAPI = nullptr;
CFilesystem* Filesystem = nullptr;
CScene* Scene = nullptr;
CUserInterface* UserInterface = nullptr;
CMainWindow* MainWindow = nullptr;
CInput* Input = nullptr;
CScheduler* Scheduler = nullptr;
CCamera* Camera = nullptr;
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

#ifdef USE_DX11
	Render = new CRenderDX11();
	//RenderBackend = new CRenderBackendDX11();
#else
	Render = new CRenderDX9();
	RenderBackend = new CRenderBackendDX9();
#endif

    Render->Initialize();

	UserInterface = new CUserInterface();
	UserInterface->Initialize();

	Camera = new CCamera();								 
	Camera->Initialize();
	
	Scene = new CScene();

	OptickAPI = new COptickAPI();
}

void CApplication::Destroy()
{
	Scene->Destroy();

	UserInterface->Destroy();
	delete UserInterface;

	delete Camera;

	Render->Destroy();
	delete Render;

	delete Input;

	delete MainWindow;

	delete Scheduler;

	Filesystem->Destroy();
	delete Filesystem;

	Log->Destroy();
	delete Log;

	OptickAPI->Destroy();
	delete OptickAPI;
}

void CApplication::HandleSDLEvents()
{
	if (SDL_PollEvent(&g_WindowEvent))
	{
		if (g_WindowEvent.type == SDL_QUIT)
			g_bNeedCloseApplication = true;

		ImGui_ImplSDL2_ProcessEvent(&g_WindowEvent);
	}
}

void InputUpdateTask()
{
	OPTICK_FRAME("InputUpdateTask")
	OPTICK_EVENT("InputUpdateTask")

	Input->OnFrame();
}

void ProfilingTask()
{
	OptickAPI->OnFrame();
}

void CApplication::OnFrame()
{
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	HandleSDLEvents();

	Scheduler->Add(InputUpdateTask);

	ProfilingTask();

	//concurrency::task_group render_task;
	//render_task.run([]() 
	//{ 
		
	//});

	Camera->OnFrame();
	Render->OnFrame();

	UserInterface->OnFrame();

	if (UserInterface->NeedLoadScene())
	{
		Scene->Load();
	}

	if (UserInterface->NeedDestroyScene())
	{
		Scene->Destroy();
		UserInterface->SetNeedDestroyScene(false);
	}

	//render_task.wait();
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

	Start();

	delete (SplashScreen);

	Msg("Application started successfully");
	Msg("\n");

	EventLoop();

	Msg("Destroying Application...");

	Destroy();
}
///////////////////////////////////////////////////////////////
