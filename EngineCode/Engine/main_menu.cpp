///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Main menu window
///////////////////////////////////////////////////////////////
#include "main_menu.h"
#include "imgui_api.h"
#include "render.h"
#include "log.h"
///////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
///////////////////////////////////////////////////////////////
CMainMenu::CMainMenu()
{
	m_bNeedDraw = false;
	m_bNeedLoadScene = false;
}

void CMainMenu::Draw()
{
	if (m_bNeedDraw)
	{
		ImGui::Begin("Main menu window");

		if (ImGui::Button("Load scene"))
			m_bNeedLoadScene = true;

		if (ImGui::Button("Close application"))
			g_bNeedCloseApplication = true;

		ImGui::End();
	}
}

void CMainMenu::Show()
{
	m_bNeedDraw = true;
}

void CMainMenu::Hide()
{
	m_bNeedDraw = false;
}

void CMainMenu::SceneLoaded()
{
	m_bNeedLoadScene = false;
}

bool CMainMenu::NeedLoadScene()
{
	return m_bNeedLoadScene;
}
///////////////////////////////////////////////////////////////
