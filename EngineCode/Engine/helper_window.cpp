///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui helping window
///////////////////////////////////////////////////////////////
#include "helper_window.h"
#include "imgui_api.h"
#include "render.h"
#include "log.h"
///////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
///////////////////////////////////////////////////////////////
bool g_bNeedRestart = false;
bool g_bWireframeMode = false;
///////////////////////////////////////////////////////////////
CHelperWindow::CHelperWindow()
{
	m_bNeedDraw = false;
	m_bNeedQuitToMainMenu = false;
}

void CHelperWindow::Draw()
{
	if (m_bNeedDraw)
	{
		ImGui::Begin("Armillary helper window");

		if (ImGui::Button("Reset render"))
			g_bNeedRestart = true;

		if (ImGui::Button("Wireframe"))
			g_bWireframeMode = !g_bWireframeMode;

		if (ImGui::Button("Flush log"))
			Log->Flush();

		if (ImGui::Button("Quit to main menu"))
			m_bNeedQuitToMainMenu = true;

		if (ImGui::Button("Close application"))
			g_bNeedCloseApplication = true;

		ImGui::End();
	}
}

void CHelperWindow::Show()
{
	m_bNeedDraw = true;
}

void CHelperWindow::Hide()
{
	m_bNeedDraw = false;
}

bool CHelperWindow::NeedQuitToMainMenu()
{
	return m_bNeedQuitToMainMenu;
}

void CHelperWindow::QuitingToMainMenuIsDone()
{
	m_bNeedQuitToMainMenu = false;
}
///////////////////////////////////////////////////////////////
