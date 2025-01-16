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
bool g_bNeedRestart = false;
bool g_bWireframeMode = false;
///////////////////////////////////////////////////////////////
void CHelperWindow::Draw()
{
	ImGui::Begin("Armillary helper window");

	if (ImGui::Button("Reset render"))
		g_bNeedRestart = true;

	if (ImGui::Button("Wireframe"))
		g_bWireframeMode = !g_bWireframeMode;

	if (ImGui::Button("Flush log"))
		Log->Flush();

#ifdef DEBUG_BUILD
		// if (ImGui::Button("Optic capture frame"))
		//	OptickAPI->StartCapturing(1);
#endif

	ImGui::End();
}
///////////////////////////////////////////////////////////////
