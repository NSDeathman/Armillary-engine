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
#include "EngineSettings.h"

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

#include "Monitoring.h"
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
EngineSettings* Settings = nullptr;
CMonitoring* Monitoring = nullptr;
///////////////////////////////////////////////////////////////
CApplication::CApplication()
{
	m_LastTime = 0.0f;
	m_CurrentTime = 0.0f;
	m_TimeDelta = 0.0f;
	m_FrameTime = 0.0f;
	m_FPS = 0.0f;
	m_Frame = 0;
}

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

	Settings = new EngineSettings();

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

	Monitoring = new CMonitoring();

	m_Timer.Start();
}

void CApplication::Destroy()
{
	Scene->Destroy();

	delete Monitoring;

	UserInterface->Destroy();
	delete UserInterface;

	delete Camera;

	Render->Destroy();
	delete Render;

	delete Input;

	delete MainWindow;

	delete Scheduler;

	delete Settings;

	Filesystem->Destroy();
	delete Filesystem;

	Log->Destroy();
	delete Log;

	OptickAPI->Destroy();
	delete OptickAPI;
}

void CApplication::HandleSDLEvents()
{
	if (SDL_PollEvent(&g_WindowEvent)) [[likely]]
	{
		if (g_WindowEvent.type == SDL_QUIT) [[unlikely]]
			g_bNeedCloseApplication = true;

		ImGui_ImplSDL2_ProcessEvent(&g_WindowEvent);
	}
}

void ProfilingTask()
{
	OptickAPI->OnFrame();
}

void CApplication::CalculateTimeStats()
{
	m_LastTime = m_CurrentTime;
	m_CurrentTime = (float)m_Timer.GetTime();

	m_TimeDelta = m_CurrentTime - m_LastTime;

	m_FPS = 1.0f / m_TimeDelta;
}

CTimer SummaryTimer;

void RenderTask()
{
	MONITOR_SCOPE(MONITORNG_CHART::Render);
	Render->OnFrame();
}

void UITask()
{
	MONITOR_SCOPE(MONITORNG_CHART::UI);
	UserInterface->OnFrame();
}

void InputUpdate()
{
	OPTICK_FRAME("InputUpdate")
	OPTICK_EVENT("InputUpdate")

	Input->OnFrame();
}

void InputTask()
{
	MONITOR_SCOPE(MONITORNG_CHART::Input);
	Scheduler->Add(InputUpdate);
}

void CApplication::OnFrame()
{
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	SummaryTimer.Start();

	HandleSDLEvents();

	InputTask();

	ProfilingTask();

	//concurrency::task_group render_task;
	//render_task.run([]() 
	//{ 
		
	//});

	Camera->OnFrame();

	UITask();

	if (UserInterface->NeedLoadScene()) [[unlikely]]
	{
		Scene->Load();
	}

	if (UserInterface->NeedDestroyScene()) [[unlikely]]
	{
		Scene->Destroy();
		UserInterface->SetNeedDestroyScene(false);
	}

	RenderTask();

	m_FrameTime = SummaryTimer.GetElapsedTime();
	SummaryTimer.Stop();

	if (m_FrameTime < 0.01666666f) [[unlikely]]
	{
		float dt = 0.01666666f - m_FrameTime;
		m_FrameTime += dt;
		std::this_thread::sleep_for(std::chrono::milliseconds(int(dt * 1000)));
	}

	CalculateTimeStats();

	Monitoring->OnFrame();

	m_Frame++;

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

	Msg("Application started successfully\n");

	EventLoop();

	Msg("Destroying Application...");

	Destroy();
}
///////////////////////////////////////////////////////////////
