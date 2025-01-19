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
	m_bHelperWndDraw = true;

	m_bKeyPressed = false;
}

void CUserInterface::OnFrameBegin()
{
	Imgui->OnFrameBegin();
}

extern bool m_bKeyPressed;

void CUserInterface::CatchInput()
{
	const u8* KeyBoardStates = SDL_GetKeyboardState(NULL);
	if (m_bKeyPressed)
		if (KeyBoardStates[SDL_SCANCODE_ESCAPE])
			Msg("esc");
}

void CUserInterface::OnFrame()
{
	CatchInput();

	if (!Scene->Ready())
		MainMenu->Show();
	else if (Scene->Ready() && m_bHelperWndDraw)
		HelperWindow->Hide();

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

		if (m_bHelperWndDraw)
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
