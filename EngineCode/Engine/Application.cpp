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
#include "build_identification_helper.h"
#include "imgui_api.h"
#include "OptickAPI.h"
#include "helper_window.h"
#include "filesystem.h"
#include "main_menu.h"
#include "scene.h"
///////////////////////////////////////////////////////////////
bool g_bNeedCloseApplication = false;
///////////////////////////////////////////////////////////////
CRender* Render = NULL;
CBackend* RenderBackend = NULL;
CLog* Log = NULL;
CImguiAPI* Imgui = NULL;
COptickAPI* OptickAPI = NULL;
CHelperWindow* HelperWindow = NULL;
CFilesystem* Filesystem = NULL;
CMainMenu* MainMenu = NULL;
CScene* Scene = NULL;
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

	PrintStartData();
	
	Msg("Starting Application...");

	Render = new(CRender);
	RenderBackend = new(CBackend);
	Imgui = new (CImguiAPI);

    Render->Initialize();
	Imgui->Initialize();

	HelperWindow = new (CHelperWindow);

	MainMenu = new (CMainMenu);

	Scene = new (CScene);
}

void CApplication::Destroy()
{
	delete (MainMenu);
	delete (HelperWindow);

	Imgui->Destroy();
	delete (Imgui);

	Scene->Destroy();

	Render->Destroy();
	delete (Render);

	Filesystem->Destroy();
	delete (Filesystem);

	Log->Destroy();
	delete (Log);
}

void RenderFrame()
{
	OPTICK_THREAD("Armillary engine render thread")
	OPTICK_FRAME("RenderThreadTask")
	OPTICK_EVENT("RenderThreadTask")

	Render->OnFrameBegin();

	MainMenu->Draw();

	HelperWindow->Draw();

	Render->RenderFrame();

	Render->OnFrameEnd();
}

void CApplication::OnFrame()
{
	OPTICK_THREAD("Armillary engine primary thread")
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	if (!Scene->Ready() && !Scene->isLoading())
		MainMenu->Show();

	if (MainMenu->NeedLoadScene())
	{
		Scene->Load();
		if (!Scene->isLoading())
		{
			MainMenu->SceneLoaded();
			MainMenu->Hide();
			HelperWindow->Show();
		}
	}

	if (HelperWindow->NeedQuitToMainMenu())
	{
		Scene->Destroy();
		HelperWindow->QuitingToMainMenuIsDone();
		HelperWindow->Hide();
	}

	RenderFrame();
}

void CApplication::EventLoop()
{
	Msg("Starting event loop...");

	MSG msg_struct;
	ZeroMemory(&msg_struct, sizeof(msg_struct));
	while (msg_struct.message != WM_QUIT && g_bNeedCloseApplication != true)
	{
		if (PeekMessage(&msg_struct, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg_struct);
			DispatchMessage(&msg_struct);
		}

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
