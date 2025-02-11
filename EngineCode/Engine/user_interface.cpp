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
#include "camera.h"
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

	SetNeedLoadScene(false);
	SetNeedDestroyScene(false);

	m_bHelperWndDraw = true;
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

void CUserInterface::UpdateIngameUI()
{
	if (Scene->isLoading() && NeedLoadScene())
	{
		SetNeedLoadScene(false);
	}

	if (Scene->Ready())
	{
		LoadingScreen->Hide();

		if (m_bHelperWndDraw)
			HelperWindow->Show();
		else
			HelperWindow->Hide();

		if (Input->KeyPressed(SDL_SCANCODE_ESCAPE) || Input->GamepadButtonPressed(SDL_CONTROLLER_BUTTON_START))
		{
			m_bHelperWndDraw = !m_bHelperWndDraw;
			g_bNeedLockCursor = !m_bHelperWndDraw;
			g_bNeedUpdateCameraInput = !m_bHelperWndDraw;
		}
	}

	if (HelperWindow->NeedQuitToMainMenu())
	{
		m_bNeedDestroyScene = true;
		HelperWindow->QuitingToMainMenuIsDone();
		HelperWindow->Hide();

		g_bNeedLockCursor = false;
		g_bNeedUpdateCameraInput = false;

		Camera->SetDefaultParams();
	}
}

void CUserInterface::OnFrame()
{
	if (MainMenu->NeedLoadScene())
	{
		SetNeedLoadScene(true);
		MainMenu->Hide();
		LoadingScreen->Show();
		MainMenu->SetNeedLoadScene(false);
	}

	if (!Scene->Ready() && 
		!NeedLoadScene() && 
		!Scene->isLoading())
	{
		MainMenu->Show();
	}
	else
	{
		UpdateIngameUI();
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
