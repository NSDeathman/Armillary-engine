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
#include "build_identification_helper.h"
#include "OptickAPI.h"
#include "filesystem.h"
#include "scene.h"
#include "user_interface.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
bool g_bNeedCloseApplication = false;
///////////////////////////////////////////////////////////////
CRender* Render = NULL;
CBackend* RenderBackend = NULL;
CLog* Log = NULL;
COptickAPI* OptickAPI = NULL;
CFilesystem* Filesystem = NULL;
CScene* Scene = NULL;
CUserInterface* UserInterface = NULL;
CMainWindow* MainWindow = NULL;
///////////////////////////////////////////////////////////////
void CApplication::PrintStartData()
{
	Msg("Armillary engine");

	u32 MajorBuildID = compute_build_id_major();
	u32 MinorBuildID = compute_build_id_minor();
	Msg("Build ID: %d.%d", MajorBuildID, MinorBuildID);

#ifdef _DEBUG
	Msg("Build type: Debug");
#else
	Msg("Build type: Release");
#endif

#ifdef WIN64
	Msg("Build architecture: Win64");
#else
	Msg("Build architecture: Win32");
#endif

	Msg("\n");
}

void CApplication::Start()
{
	Filesystem = new (CFilesystem);
	Log = new (CLog);

	_initialize_cpu();

	PrintStartData();
	
	Msg("Starting Application...");

	MainWindow = new (CMainWindow);

	Render = new(CRender);
	RenderBackend = new(CBackend);

    Render->Initialize();

	UserInterface = new (CUserInterface);
	UserInterface->Initialize();

	Scene = new (CScene);
}

void CApplication::Destroy()
{
	Scene->Destroy();

	UserInterface->Destroy();
	delete (UserInterface);

	Render->Destroy();
	delete (Render);

	Filesystem->Destroy();
	delete (Filesystem);

	Log->Destroy();
	delete (Log);

	delete (MainWindow);
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

void CatchInput()
{
	SDL_Event localEvent;

	// Poll events
	while (SDL_PollEvent(&localEvent))
	{
		if (localEvent.type == SDL_KEYUP)
		{
			// Reset key pressed status on key release
			if (localEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				m_bKeyPressed = false;
			}
		}
	}

	// Optionally: Check the keyboard state directly for continuous input
	const u8* KeyBoardStates = SDL_GetKeyboardState(NULL);
	if (KeyBoardStates[SDL_SCANCODE_ESCAPE] && !m_bKeyPressed)
	{
		Msg("Escape is currently pressed");
		m_bKeyPressed = true; // Set to true to prevent repeating messages
	}
}

void CApplication::EventLoop()
{
	Msg("Starting event loop...");

	SDL_Event WindowEvent;

	while (!g_bNeedCloseApplication)
	{
		// Handle window events
		if (SDL_PollEvent(&WindowEvent))
		{
			if (WindowEvent.type == SDL_QUIT)
				break;
		}

		CatchInput();							   // Catch keyboard input here
		OnFrame();								   // Your frame render/update logic
		ImGui_ImplSDL2_ProcessEvent(&WindowEvent); // Handle ImGui events
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
