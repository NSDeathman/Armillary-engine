///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui helping window
///////////////////////////////////////////////////////////////
#include "helper_window.h"
#include "imgui_api.h"
#include "render.h"
#include "log.h"
#include "camera.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
///////////////////////////////////////////////////////////////
bool g_bNeedRestart = false;
bool g_bWireframeMode = false;
///////////////////////////////////////////////////////////////
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
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
}

void CHelperWindow::Draw()
{
	if (m_bNeedDraw)
	{
		ImGui::PushFont(Imgui->font_letterica_big);
		ImGui::Begin("Helper window");
		ImGui::PopFont();

		ImGui::PushFont(Imgui->font_letterica_medium);

		if (ImGui::TreeNode("Settings"))
		{
			if (ImGui::TreeNode("Camera settings"))
			{
				ImGui::DragFloat("Camera fov", &g_Fov, 1.0f, 30.0f, 130.0f);
				ImGui::DragFloat("View distance", &g_FarPlane, 10.0f, 50.0f, 300.0f);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Render settings"))
			{
				if (ImGui::Combo("Screen resolution", &SelectedResolution, screen_resolution_list, IM_ARRAYSIZE(screen_resolution_list)))
					ChangeScreenResolution();

				if (ImGui::Button("Reset render"))
					g_bNeedRestart = true;

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
			ImGui::TreePop();
		}

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
