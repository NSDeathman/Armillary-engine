///////////////////////////////////////////////////////////////
// Created: 19.09.2025
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "architect_patterns.h"
#include "splash_screen.h"
#include "window.h"
#include "TimeSystem.h"
#include "log.h"
#include "filesystem.h"
#include "build_identificator.h"
#include "cpu_identificator.h"
#include "AsyncExecutor.h"
#include "Input.h"
#include "architect_patterns.h"
#include "Render.h"
#include "imgui_api.h"
#include "DebugAPI.h"
#include "CoreMacros.h"
///////////////////////////////////////////////////////////////
CORE_BEGIN
class CCoreAPI : public Patterns::Singleton<CCoreAPI>
{
	friend class Core::Patterns::Singleton<CCoreAPI>;

	public:
	// Classes
	CTimeSystem TimeSystem;
	CLog Logger;
	CAsyncExecutor AsyncExecutor;
	CInput Input;
	CFilesystem Filesystem;

	void Initialize();
	void Destroy();

	private:
	CCoreAPI() = default;
	~CCoreAPI();
};
CORE_END
///////////////////////////////////////////////////////////////
#define CoreAPI Core::CCoreAPI::GetInstance()
///////////////////////////////////////////////////////////////
#define Print CoreAPI.Logger.PrintInternal
#define PrintWithLevel CoreAPI.Logger.PrintInternalWithLevel
#define DebugPrint CoreAPI.Logger.Debug_Print
#define PrintWarning CoreAPI.Logger.Warning_Print
#define PrintError CoreAPI.Logger.Error_Print
///////////////////////////////////////////////////////////////
#define KEY_PRESSED(key) CoreAPI.Input.IsKeyPressed(key)
#define KEY_HELD(key) CoreAPI.Input.IsKeyHeld(key)
#define KEY_RELEASED(key) CoreAPI.Input.IsKeyReleased(key)
#define GAMEPAD_BUTTON_PRESSED(button) CoreAPI.Input.IsGamepadButtonPressed(button)
#define GAMEPAD_BUTTON_HELD(button) CoreAPI.Input.IsGamepadButtonHeld(button)
#define GAMEPAD_BUTTON_RELEASED(button) CoreAPI.Input.IsGamepadButtonReleased(button)
#define MOUSE_BUTTON_PRESSED(button) CoreAPI.Input.IsMouseButtonPressed(button)
#define MOUSE_BUTTON_HELD(button) CoreAPI.Input.IsMouseButtonHeld(button)
///////////////////////////////////////////////////////////////
