///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui implementation
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CImguiAPI
{
public:
	ImFont* font_letterica;
	ImFont* font_letterica_small;
	ImFont* font_letterica_medium;
	ImFont* font_letterica_big;

	ImFont* font_maven_pro_back;
	ImFont* font_maven_pro_bold;
	ImFont* font_maven_pro_medium;
	ImFont* font_maven_pro_regular;

private:
	ImGuiIO m_pImGuiInputOutputParams;

public:
	CImguiAPI() = default;
	~CImguiAPI() = default;

	void Initialize();
	void OnFrameBegin();
	void RenderFrame();
	void OnFrameEnd();
	void OnResetBegin();
	void OnResetEnd();
	void Destroy();
	void HideCursor();
	void ShowCursor();
};
///////////////////////////////////////////////////////////////
extern CImguiAPI* Imgui;
///////////////////////////////////////////////////////////////
