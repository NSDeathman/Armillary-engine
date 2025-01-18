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
#include "loading_screen_window.h"
#include "scene.h"
///////////////////////////////////////////////////////////////
CImguiAPI* Imgui = NULL;
CHelperWindow* HelperWindow = NULL;
CMainMenu* MainMenu = NULL;
CLoadingScreen* LoadingScreen = NULL;
///////////////////////////////////////////////////////////////
void CUserInterface::Initialize()
{
	Imgui = new (CImguiAPI);
	Imgui->Initialize();

	MainMenu = new (CMainMenu);
	HelperWindow = new (CHelperWindow);
	LoadingScreen = new (CLoadingScreen);

	m_bNeedLoadScene = false;
	m_bNeedDestroyScene = false;
}

void CUserInterface::OnFrameBegin()
{
	Imgui->OnFrameBegin();
}

void CUserInterface::OnFrame()
{
	if (!Scene->Ready())
		MainMenu->Show();


	if (MainMenu->NeedLoadScene())
	{
		MainMenu->Hide();
		LoadingScreen->Show();
		m_bNeedLoadScene = true;
	}
	
	if (Scene->isLoading())
		m_bNeedLoadScene = false;

	if (Scene->Ready())
	{
		MainMenu->SceneLoaded();
		LoadingScreen->Hide();
		HelperWindow->Show();
	}

	if (HelperWindow->NeedQuitToMainMenu())
	{
		m_bNeedDestroyScene = true;
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
	LoadingScreen->Draw();
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
	delete (LoadingScreen);

	Imgui->Destroy();
	delete (Imgui);
}
///////////////////////////////////////////////////////////////
