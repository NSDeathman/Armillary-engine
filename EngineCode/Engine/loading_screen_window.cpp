///////////////////////////////////////////////////////////////
// Created: 18.01.2025
// Author: NS_Deathman
// Loading screen window
///////////////////////////////////////////////////////////////
#include "loading_screen_window.h"
#include "imgui_api.h"
#include "log.h"
///////////////////////////////////////////////////////////////
CLoadingScreen::CLoadingScreen()
{
	m_bNeedDraw = false;
}

void CLoadingScreen::Draw()
{
	if (m_bNeedDraw)
	{
		ImGui::PushFont(Imgui->font_letterica_big);
		ImGui::Begin("Loading screen window");
		ImGui::PopFont();

		ImGui::PushFont(Imgui->font_letterica_medium);

		ImGui::Text("Loading scene");

		ImGui::PopFont();

		ImGui::End();
	}
}

void CLoadingScreen::Show()
{
	m_bNeedDraw = true;
}

void CLoadingScreen::Hide()
{
	m_bNeedDraw = false;
}
///////////////////////////////////////////////////////////////
