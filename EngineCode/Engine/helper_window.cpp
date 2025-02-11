///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui helping window
///////////////////////////////////////////////////////////////
#include "helper_window.h"
#include "imgui_api.h"
#include "log.h"
#include "camera.h"
#include "main_window.h"

#ifdef USE_DX11
#include "render_DX11.h"
#else
#include "render_DX9.h"
#endif
///////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
///////////////////////////////////////////////////////////////
bool g_bNeedRestart = false;
bool g_bWireframeMode = true;
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
static int SelectedResolution = 0;
const char* screen_resolution_list[] = 
{
	"854x480 FWVGA",
	"720x480 SD", 
	"800x600 SVGA",
	"1280x720 HD", 
	"1280x800 WXGA",
	"1366x760 WXGA",
	"1600x900 HD+",
	"1920x1080 FULL HD",
};

enum 
{
	FWVGA = 0,
	SD = 1,
	SVGA = 2,
	HD = 3,
	WXGA0 = 4,
	WXGA1 = 5,
	HDPLUS = 6,
	FULLHD = 7,
};

void ChangeScreenResolution()
{
	switch (SelectedResolution)
	{
	case FWVGA:
		g_ScreenWidth = 800;
		g_ScreenHeight = 480;
		break;
	case SD:
		g_ScreenWidth = 720;
		g_ScreenHeight = 480;
		break;
	case SVGA:
		g_ScreenWidth = 800;
		g_ScreenHeight = 600;
		break;
	case HD:
		g_ScreenWidth = 1280;
		g_ScreenHeight = 720;
		break;
	case WXGA0:
		g_ScreenWidth = 1280;
		g_ScreenHeight = 800;
		break;
	case WXGA1:
		g_ScreenWidth = 1366;
		g_ScreenHeight = 768;
		break;
	case HDPLUS:
		g_ScreenWidth = 1600;
		g_ScreenHeight = 900;
		break;
	case FULLHD:
		g_ScreenWidth = 1920;
		g_ScreenHeight = 1080;
		break;
	}

	Render->SetNeedReset();
}
///////////////////////////////////////////////////////////////
CHelperWindow::CHelperWindow()
{
	m_bNeedDraw = false;
	m_bNeedQuitToMainMenu = false;
	m_bNeedDrawSettings = false;
}

void CHelperWindow::DrawSettings()
{
	ImGui::PushFont(Imgui->font_letterica_big);
	ImGui::Begin("Settings window", &m_bNeedDrawSettings);
	ImGui::PopFont();

	ImGui::PushFont(Imgui->font_letterica_medium);

	if (ImGui::TreeNode("Camera settings"))
	{
		if (!g_UseOrthogonalProjection)
		{
			if (ImGui::Button("Orthographic camera"))
				g_UseOrthogonalProjection = true;

			if(ImGui::DragFloat("Camera fov", &g_Fov, 0.5f, 30.0f, 130.0f))
				Camera->SetFov(g_Fov);
		}
		else 
		{
			if (ImGui::Button("Perspective camera"))
				g_UseOrthogonalProjection = false;

			ImGui::DragFloat("Orthogonal size", &g_OrthogonalProjectionSize, 0.01f, 1.0f, 4.0f);
		}

		if(ImGui::DragFloat("View distance", &g_FarPlane, 5.0f, 50.0f, 300.0f))
			Camera->SetFarPlane(g_FarPlane);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Render settings"))
	{
		if (ImGui::Combo("Screen resolution", &SelectedResolution, screen_resolution_list, IM_ARRAYSIZE(screen_resolution_list)))
			ChangeScreenResolution();

		if (ImGui::Button("Reset render"))
			Render->SetNeedReset();

		if (ImGui::Button("Wireframe"))
			g_bWireframeMode = !g_bWireframeMode;

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("System"))
	{
		if (ImGui::Button("Flush log"))
			Log->Flush();

		ImGui::TreePop();
	}

	ImGui::PopFont();

	ImGui::End();
}

void CHelperWindow::Draw()
{
	if (m_bNeedDraw)
	{
		if (m_bNeedDrawSettings)
			DrawSettings();

		ImGui::PushFont(Imgui->font_letterica_big);
		ImGui::Begin("Helper window");
		ImGui::PopFont();

		ImGui::PushFont(Imgui->font_letterica_medium);

		if (ImGui::Button("Settings"))
			m_bNeedDrawSettings = !m_bNeedDrawSettings;

		if (ImGui::Button("Quit to main menu"))
			m_bNeedQuitToMainMenu = true;

		if (ImGui::Button("Close application"))
			g_bNeedCloseApplication = true;

		ImGui::PopFont();

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
