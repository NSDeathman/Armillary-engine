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
