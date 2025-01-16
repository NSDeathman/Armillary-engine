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
#include "threading.h"
#include "imgui_api.h"
///////////////////////////////////////////////////////////////
CRender* Render = NULL;
CBackend* RenderBackend = NULL;
CLog* Log = NULL;
CImguiAPI* Imgui = NULL;
///////////////////////////////////////////////////////////////
void CApplication::PrintStartData()
{
	Log->Print("Atlas engine");

	u32 MajorBuildID = compute_build_id_major();
	u32 MinorBuildID = compute_build_id_minor();
	Log->Print("Build ID: %d.%d", MajorBuildID, MinorBuildID);

#ifdef _DEBUG
	Log->Print("Build type: Debug");
#else
	Log->Print("Build type: Release");
#endif

#ifdef WIN32
	Log->Print("Build architecture: Win32");
#else
	Log->Print("Build architecture: Win64");
#endif

	Log->Print("\n");
}

void CApplication::Start()
{
	Log = new (CLog);

	PrintStartData();
	
	Log->Print("Starting Application...");

	Render = new(CRender);
	RenderBackend = new(CBackend);
	Imgui = new (CImguiAPI);

    Render->Initialize();
	Imgui->Initialize();
}

void RenderThreadTask()
{
	OPTICK_THREAD("Atlas render thread")
	OPTICK_FRAME("RenderThreadTask")
	OPTICK_EVENT("RenderThreadTask")

	Render->OnFrame();
}

void CApplication::OnFrame()
{
	OPTICK_THREAD("Atlas primary thread")
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	//Scheduler.Add(ThreadWork0);

	//concurrency::task_group task_render;
	//task_render.run([&]() 
	//{ 
	//	
	//});

	RenderThreadTask();

	//task_render.wait();
}

void CApplication::EventLoop()
{
	Log->Print("Starting event loop...");

	MSG msg_struct;
	ZeroMemory(&msg_struct, sizeof(msg_struct));
	while (msg_struct.message != WM_QUIT)
	{
		if (PeekMessage(&msg_struct, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg_struct);
			DispatchMessage(&msg_struct);
		}
		else
		{
			OnFrame();
		}
	}
}

void CApplication::Process()
{
	SplashScreen = new (CSplashScreen);

	App->Start();

	delete (SplashScreen);

	Log->Print("Application started successfully");
	Log->Print("\n");

	App->EventLoop();

	Log->Print("Destroying Application...");

	App->Destroy();
}

void CApplication::Destroy()
{
	Log->Print("Destroying application...");

	Imgui->Destroy();
	delete (Imgui);

	Render->Destroy();
	delete (Render);

	delete (Log);
}
///////////////////////////////////////////////////////////////
