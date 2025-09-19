///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Main menu window
///////////////////////////////////////////////////////////////
#include "main_menu.h"
#include "imgui_api.h"
#include "log.h"
#include "input.h"
#include "render_DX9.h"
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
	if (!m_bNeedDraw)
		return;

	ImGui::PushFont(Imgui->font_letterica_big);
	ImGui::Begin("Main menu window");
	ImGui::PopFont();

	ImGui::PushFont(Imgui->font_letterica_medium);

	if (ImGui::Button("Load scene"))
		SetNeedLoadScene(true);

	if (ImGui::Button("Close application"))
		g_bNeedCloseApplication = true;

	ImGui::PopFont();

	ImGui::End();
}

void CMainMenu::Show()
{
	m_bNeedDraw = true;
	Input->SetNeedUpdateCursorWithGameController(true);
}

void CMainMenu::Hide()
{
	m_bNeedDraw = false;
	Input->SetNeedUpdateCursorWithGameController(false);
}

void CMainMenu::SetNeedLoadScene(bool flag)
{
	m_bNeedLoadScene = flag;
}

bool CMainMenu::NeedLoadScene()
{
	return m_bNeedLoadScene;
}
///////////////////////////////////////////////////////////////
