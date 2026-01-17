///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui implementation (Refactored for New Engine)
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "CoreMacros.h"
#include "architect_patterns.h" // Для Singleton
///////////////////////////////////////////////////////////////
// ImGui Headers
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/ImGuizmo.h>
#include <imgui/ImCurveEdit.h>
#include <imgui/ImGradient.h>
#include <imgui/ImSequencer.h>
#include <imgui/GraphEditor.h>
///////////////////////////////////////////////////////////////
CORE_BEGIN

class CImguiAPI : public Core::Patterns::Singleton<CImguiAPI>
{
	friend class Core::Patterns::Singleton<CImguiAPI>;

  public:
	// Шрифты
	ImFont* font_letterica = nullptr;
	ImFont* font_letterica_small = nullptr;
	ImFont* font_letterica_medium = nullptr;
	ImFont* font_letterica_big = nullptr;

	ImFont* font_maven_pro_black = nullptr;
	ImFont* font_maven_pro_bold = nullptr;
	ImFont* font_maven_pro_medium = nullptr;
	ImFont* font_maven_pro_regular = nullptr;

  public:
	void Initialize();
	void Destroy();

	// Жизненный цикл кадра
	void OnFrameBegin(); // Вызывать в начале кадра (перед апдейтом игры)
	void RenderFrame();	 // Вызывать перед Present()

	// Управление курсором
	void HideCursor();
	void ShowCursor();

	// Обработка событий (для SDL2)
	void ProcessEvent(const SDL_Event* event);

  private:
	CImguiAPI() = default;
	~CImguiAPI();

	bool m_Initialized = false;
};

CORE_END
///////////////////////////////////////////////////////////////
#define IMGUI Core::CImguiAPI::GetInstance()
///////////////////////////////////////////////////////////////
