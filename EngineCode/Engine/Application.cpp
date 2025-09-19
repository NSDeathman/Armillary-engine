///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
// Application and stdafx
#include "Application.h"

// Main engine parts
#include "render_DX9.h"
#include "render_backend_DX9.h"
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
#include "AsyncExecutor.h"

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
CApplication::CApplication()
{
	m_LastTime = 0.0f;
	m_CurrentTime = 0.0f;
	m_TimeDelta = 0.0f;
	m_FrameTime = 0.0f;
	m_FPS = 0.0f;
	m_FPSLimit = 60.0f;
	m_Frame = 0;
}

void CApplication::Start()
{
	SplashScreen = new (CSplashScreen);
	SplashScreen->Show();

	Filesystem = new CFilesystem();

	Log = new CLog();

	initializeCPU();

	PrintBuildData();
	
	Msg("Starting Application...");

	AsyncExecutor = new CAsyncExecutor();

	Input = new CInput();

	Settings = new EngineSettings();

	OptickAPI = new COptickAPI();

	Monitoring = new CMonitoring();

	MainWindow = new CMainWindow();

	Render = new CRenderDX9();
	RenderBackend = new CRenderBackendDX9();
    Render->Initialize();

	UserInterface = new CUserInterface();
	UserInterface->Initialize();

	Camera = new CCamera();								 
	Camera->Initialize();
	
	Scene = new CScene();

	m_Timer.Start();

	SplashScreen->Hide();
	delete (SplashScreen);
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

	delete AsyncExecutor;

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

void CApplication::CalculateTimeStats()
{
	m_LastTime = m_CurrentTime;
	m_CurrentTime = (float)m_Timer.GetTime();

	m_TimeDelta = m_CurrentTime - m_LastTime;

	m_FPS = 1.0f / m_TimeDelta;
}

void CApplication::ProcessFrameLimiter()
{
	float LimitFrametime = 1.0f / m_FPSLimit;

	if (m_FrameTime < LimitFrametime) [[unlikely]]
	{
		float dt = LimitFrametime - m_FrameTime;
		m_FrameTime += dt;
		std::this_thread::sleep_for(std::chrono::milliseconds(int(dt * 1000)));
	}
}

CTimer SummaryTimer;

void CApplication::OnFrame()
{
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	SummaryTimer.Start();

	HandleSDLEvents();

	concurrency::task_group input_task;
	input_task.run([]()
	{ 
		MONITOR_SCOPE(MONITORNG_CHART::Input);
		Input->OnFrame();
	});

	// UI
	{
		MONITOR_SCOPE(MONITORNG_CHART::UI);
		UserInterface->OnFrame();
	}

	input_task.wait();

	Camera->OnFrame();

	Scene->OnFrame();

	// Rendering
	{
		MONITOR_SCOPE(MONITORNG_CHART::Render);
		Render->OnFrame();
	}

	m_FrameTime = SummaryTimer.GetElapsedTime();
	SummaryTimer.Stop();

	ProcessFrameLimiter();

	CalculateTimeStats();

	Monitoring->OnFrame();

	OptickAPI->OnFrame();
}

void CApplication::EventLoop()
{
	Msg("Starting event loop...");
	Msg("\n");

	while (!g_bNeedCloseApplication)
	{
		OnFrame();
		m_Frame++;
	}

	Msg("\n");
	Msg("Ending event loop...");
}   

void CApplication::Process()
{
	Start();

	Msg("Application started successfully\n");

	EventLoop();

	Msg("Destroying Application...");

	Destroy();
}
///////////////////////////////////////////////////////////////
