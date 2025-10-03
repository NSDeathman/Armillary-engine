///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui helping window
///////////////////////////////////////////////////////////////
#include "helper_window.h"
#include "imgui_api.h"
#include "log.h"
#include "camera.h"
#include "window_implementation.h"
#include "input.h"
#include "render_DX9.h"
#include "OptickAPI.h"
#include "EngineSettings.h"
#include "Application_Deprecated.h"
#include "Monitoring.h"
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
///////////////////////////////////////////////////////////////
bool g_bNeedRestart = false;
bool g_bWireframeMode = false;
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
int SelectedResolution = 0;

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

	RenderDeprecated->SetNeedReset();
}

CHelperWindow::CHelperWindow()
{
	m_bNeedDraw = false;
	m_bNeedQuitToMainMenu = false;
	m_bNeedDrawSettings = false;
	m_bNeedDrawProfilingSettings = false;
	m_bNeedLeaveToScene = false;
}

void CHelperWindow::LoadSettings()
{
	float FPSLimit = Settings->GetConfig()->GetFloat(string("engine_settings"), string("fps_limit"), 60.0f);
	Application->SetFPSLimit(FPSLimit);

	SelectedResolution = Settings->GetConfig()->GetInt(string("render_settings"), string("screen_resolution"), SelectedResolution);
	ChangeScreenResolution();

	RenderDeprecated->Anisotropy = Settings->GetConfig()->GetInt(string("render_settings"), string("anisotropy"), RenderDeprecated->Anisotropy);

	g_Fov = Settings->GetConfig()->GetFloat(string("camera_settings"), string("fov"), g_Fov);
	g_FarPlane = Settings->GetConfig()->GetFloat(string("camera_settings"), string("far_plane"), g_FarPlane);

	float mouse_sens = Settings->GetConfig()->GetFloat(string("input_settings"), string("mouse_sensivity"), INPUT.GetMouseSensitivity());
	INPUT.SetMouseSensitivity(mouse_sens);

	float gamepad_sens = Settings->GetConfig()->GetFloat(string("input_settings"), string("gamepad_sensivity"), INPUT.GetGamepadSensitivity());
	INPUT.SetGamepadSensitivity(gamepad_sens);

	float gamepad_deadzone = Settings->GetConfig()->GetFloat(string("input_settings"), string("gamepad_deadzone"), INPUT.GetGamepadDeadzone());
	INPUT.SetGamepadDeadzone(gamepad_deadzone);
}

void CHelperWindow::SaveSettings()
{
	Settings->GetConfig()->SetFloat(string("engine_settings"), string("fps_limit"), Application->GetFPSLimit());
	
	Settings->GetConfig()->SetInt(string("render_settings"), string("screen_resolution"), SelectedResolution);
	Settings->GetConfig()->SetInt(string("render_settings"), string("anisotropy"), RenderDeprecated->Anisotropy);

	Settings->GetConfig()->SetFloat(string("camera_settings"), string("fov"), g_Fov);
	Settings->GetConfig()->SetFloat(string("camera_settings"), string("far_plane"), g_FarPlane);

	Settings->GetConfig()->SetFloat(string("input_settings"), string("mouse_sensivity"), INPUT.GetMouseSensitivity());
	Settings->GetConfig()->SetFloat(string("input_settings"), string("gamepad_sensivity"), INPUT.GetGamepadSensitivity());
	Settings->GetConfig()->SetFloat(string("input_settings"), string("gamepad_deadzone"), INPUT.GetGamepadDeadzone());
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

		if (ImGui::DragInt("Anisotropy", &RenderDeprecated->Anisotropy, 1, 0, RenderDeprecated->MaxAnisotropy))
			RenderDeprecated->SetAnisotropy(RenderDeprecated->Anisotropy);

		if (ImGui::Button("Reset render"))
			RenderDeprecated->SetNeedReset();

		if (ImGui::Button("Wireframe"))
			g_bWireframeMode = !g_bWireframeMode;

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Input"))
	{
		float mouse_sens = INPUT.GetMouseSensitivity();
		if (ImGui::DragFloat("Mouse sensivity", &mouse_sens, 0.1f, 0.0f, 1.0f))
			INPUT.SetMouseSensitivity(mouse_sens);

		float gamepad_sens = INPUT.GetGamepadSensitivity();
		if (ImGui::DragFloat("Gamepad sensivity", &gamepad_sens, 0.1f, 0.0f, 1.0f))
			INPUT.SetGamepadSensitivity(gamepad_sens);

		float gamepad_deadzone = INPUT.GetGamepadDeadzone();
		if (ImGui::DragFloat("Gamepad deadzone", &gamepad_deadzone, 0.1f, 0.0f, 0.7f))
			INPUT.SetGamepadDeadzone(gamepad_deadzone);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("System"))
	{
		float FPSLimit = Application->GetFPSLimit();
		if (ImGui::DragFloat("FPS Limit", &FPSLimit, 5.0f, 30.0f, 120.0f))
			Application->SetFPSLimit(FPSLimit);

		if (ImGui::Button("Flush log"))
			LogFlush();

		if (ImGui::Button("Save settings"))
		{
			SaveSettings();
			Settings->Save();
		}

		ImGui::TreePop();
	}

	ImGui::PopFont();

	ImGui::End();
}

void CHelperWindow::DrawProfilingSettings()
{
	ImGui::PushFont(Imgui->font_letterica_big);
	ImGui::Begin("Profiling settings window", &m_bNeedDrawProfilingSettings);
	ImGui::PopFont();

	ImGui::PushFont(Imgui->font_letterica_medium);

	if (ImGui::Button("Monitoring"))
	{
		bool flag = Monitoring->GetNeedDrawMonitoring();
		flag = !flag;
		Monitoring->SetNeedDrawMonitoring(flag);
	}

	if (ImGui::Button("Capture 10 frames"))
		OptickAPI->StartCapturing(10);	

	ImGui::PopFont();

	ImGui::End();
}

void CHelperWindow::Draw()
{
	if (!m_bNeedDraw)
		return;

	if (m_bNeedDrawSettings)
		DrawSettings();

	if (m_bNeedDrawProfilingSettings)
		DrawProfilingSettings();

	ImGui::PushFont(Imgui->font_letterica_big);
	ImGui::Begin("Helper window");
	ImGui::PopFont();

	ImGui::PushFont(Imgui->font_letterica_medium);

	if (ImGui::Button("Leave to scene"))
		m_bNeedLeaveToScene = true;

	if (ImGui::Button("Settings"))
		m_bNeedDrawSettings = !m_bNeedDrawSettings;

	if (ImGui::Button("Profiling"))
		m_bNeedDrawProfilingSettings = !m_bNeedDrawProfilingSettings;

	if (ImGui::Button("Quit to main menu"))
		m_bNeedQuitToMainMenu = true;

	if (ImGui::Button("Close application"))
		g_bNeedCloseApplication = true;

	ImGui::PopFont();

	ImGui::End();
}

void CHelperWindow::Show()
{
	m_bNeedDraw = true;
	INPUT.SetHandleCursorWithGameController(true);
}

void CHelperWindow::Hide()
{
	m_bNeedDraw = false;
	INPUT.SetHandleCursorWithGameController(false);
}

bool CHelperWindow::NeedQuitToMainMenu()
{
	return m_bNeedQuitToMainMenu;
}

bool CHelperWindow::NeedLeaveToScene()
{
	return m_bNeedLeaveToScene;
}

void CHelperWindow::LeavingToSceneIsDone()
{
	m_bNeedLeaveToScene = false;
}

void CHelperWindow::QuitingToMainMenuIsDone()
{
	m_bNeedQuitToMainMenu = false;
}
///////////////////////////////////////////////////////////////
