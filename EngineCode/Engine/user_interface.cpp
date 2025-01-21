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
#include "log.h"
#include "Input.h"
///////////////////////////////////////////////////////////////
CImguiAPI* Imgui = nullptr;
CHelperWindow* HelperWindow = nullptr;
CMainMenu* MainMenu = nullptr;
CLoadingScreen* LoadingScreen = nullptr;
///////////////////////////////////////////////////////////////
void CUserInterface::Initialize()
{
	Imgui = new CImguiAPI();
	Imgui->Initialize();

	MainMenu = new CMainMenu();
	HelperWindow = new CHelperWindow();
	LoadingScreen = new CLoadingScreen();

	m_bNeedLoadScene = false;
	m_bNeedDestroyScene = false;
	m_bHelperWndDraw = true;
	m_bKeyPressed = false;
}

void CUserInterface::Destroy()
{
	delete MainMenu;
	delete HelperWindow;
	delete LoadingScreen;
	Imgui->Destroy();
	delete Imgui;
}

void CUserInterface::OnFrameBegin()
{
	Imgui->OnFrameBegin();
}

void CUserInterface::OnFrame()
{
	if (!Scene->Ready() && !MainMenu->NeedLoadScene())
	{
		MainMenu->Show();
	}
	else
	{
		if (MainMenu->NeedLoadScene())
		{
			MainMenu->Hide();
			LoadingScreen->Show();
			m_bNeedLoadScene = true;
		}

		if (Scene->isLoading())
		{
			m_bNeedLoadScene = false;
		}

		if (Scene->Ready())
		{
			MainMenu->SceneLoaded();
			LoadingScreen->Hide();

			if (m_bHelperWndDraw)
				HelperWindow->Show();
			else
				HelperWindow->Hide();

			if (Input->KeyPressed(SDL_SCANCODE_ESCAPE))
				m_bHelperWndDraw = !m_bHelperWndDraw;
		}

		if (HelperWindow->NeedQuitToMainMenu())
		{
			m_bNeedDestroyScene = true;
			HelperWindow->QuitingToMainMenuIsDone();
			HelperWindow->Hide();
		}
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
///////////////////////////////////////////////////////////////
