///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
#include "Application.h"
#include "render.h"
#include "log.h"
#include "build_identification_helper.h"
///////////////////////////////////////////////////////////////
CRender* Render = NULL;
CLog* Log = NULL;
///////////////////////////////////////////////////////////////
void CApplication::Destroy()
{
	Log->Print("Destroying application...");

	delete(Render);
	delete(Log);
}

void CApplication::Start()
{
	Log = new(CLog);

	Log->Print("Atlas engine");

	u32 BuildID = compute_build_id();
	Log->Print("BuildID %d", BuildID);

#ifdef _DEBUG
	Log->Print("Build type: Debug");
#else
	Log->Print("Build type: Release");
#endif

	Log->Print("\n");

	Log->Print("Starting Application...");

	Render = new(CRender);

    Render->Initialize();
}

void CApplication::OnFrame()
{
	OPTICK_THREAD("Atlas primary thread")
	OPTICK_FRAME("CApplication::OnFrame")
	OPTICK_EVENT("CApplication::OnFrame")

	Render->RenderFrame();
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
///////////////////////////////////////////////////////////////
