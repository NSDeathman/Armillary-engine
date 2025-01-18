///////////////////////////////////////////////////////////////
// Created: 18.01.2025
// Author: NS_Deathman
// User interface class
///////////////////////////////////////////////////////////////
#include "user_interface.h"
///////////////////////////////////////////////////////////////
#include "imgui_api.h"
#include "main_menu.h"
#include "helper_window.h"
#include "scene.h"
///////////////////////////////////////////////////////////////
CImguiAPI* Imgui = NULL;
CHelperWindow* HelperWindow = NULL;
CMainMenu* MainMenu = NULL;
///////////////////////////////////////////////////////////////
void CUserInterface::Initialize()
{
	Imgui = new (CImguiAPI);
	Imgui->Initialize();

	MainMenu = new (CMainMenu);
	HelperWindow = new (CHelperWindow);
}

void CUserInterface::OnFrameBegin()
{
	Imgui->OnFrameBegin();
}

void CUserInterface::OnFrame()
{
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
}

void CUserInterface::OnFrameEnd()
{
	Imgui->RenderFrame();
}

void CUserInterface::Render()
{
	MainMenu->Draw();
	HelperWindow->Draw();
}

void CUserInterface::OnResetBegin()
{
	Imgui->OnResetBegin();
}

void CUserInterface::OnResetEnd()
{
	Imgui->OnResetEnd();
}

void CUserInterface::Destroy()
{
	delete (MainMenu);
	delete (HelperWindow);

	Imgui->Destroy();
	delete (Imgui);
}
///////////////////////////////////////////////////////////////
