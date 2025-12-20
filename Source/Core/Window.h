///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Modified: 27.12.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <windows.h>
#include <string>
#include <functional>
#include "Core.h"
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
///////////////////////////////////////////////////////////////

namespace Core
{
enum class WindowMode
{
	Windowed,
	Borderless,
	Fullscreen
};

enum class CursorMode
{
	Normal,
	Hidden,
	Confined
};

class CORE_API CWindow
{
  public:
	struct Config
	{
		uint32_t Width = 800;
		uint32_t Height = 600;
		Math::float2 Position = Math::float2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		WindowMode Mode = WindowMode::Borderless;
		CursorMode Cursor = CursorMode::Normal;
		std::string Name = "Window";
		bool Resizable = false;
		bool VSync = false;
		bool AlwaysOnTop = false;
		uint32_t DisplayIndex = 0;
		bool AllowHighDPI = true;
	};

	using EventCallback = std::function<void(SDL_Event&)>;

	void Initialize(const Config& config);
	void Destroy();

	void Show();
	void Hide();
	void Minimize();
	void Maximize();
	void Restore();
	void Close();
	void Focus();
	bool HasFocus() const;
	bool IsMinimized() const;
	bool IsMaximized() const;
	bool IsVisible() const;

	void SetMode(WindowMode mode);
	void SetResolution(uint32_t width, uint32_t height);
	void SetPosition(int x, int y);
	void SetTitle(const std::string& title);
	void SetIcon(const std::string& iconPath);
	void SetVSync(bool enabled);
	void SetResizable(bool resizable);
	void SetAlwaysOnTop(bool alwaysOnTop);
	void SetCursorMode(CursorMode mode);
	void SetMouseCapture(bool capture);
	void SetBrightness(float brightness);

	Math::float2 GetSize() const;
	Math::float2 GetPosition() const;
	Math::float2 GetDrawableSize() const;
	float GetBrightness() const;
	float GetOpacity() const;
	void SetOpacity(float opacity);
	std::string GetTitle() const;
	WindowMode GetMode() const;
	uint32_t GetDisplayIndex() const;

	HWND GetWindowHandle() const;
	SDL_Window* GetSDLWindow() const;
	SDL_DisplayMode GetCurrentDisplayMode() const;
	std::vector<SDL_DisplayMode> GetAvailableDisplayModes() const;

	void Center();
	void CenterOnDisplay(uint32_t displayIndex);

	void SetEventCallback(const EventCallback& callback);
	void ProcessEvents();

	void TakeScreenshot(const std::string& filepath);
	void Flash(bool flash = true);
	void WarpMouse(int x, int y);
	Math::float2 GetMousePosition() const;

	void Reset(const Config& config);

	CWindow();
	~CWindow();

  private:
	SDL_Window* m_window;
	SDL_SysWMinfo m_windowInfo;
	SDL_DisplayMode m_displayMode;
	SDL_Cursor* m_cursor;
	Config m_config;
	EventCallback m_eventCallback;
	bool m_initialized;
	uint32_t m_windowID;

	// Внутренние методы
	void Create();
	void ApplyWindowMode();
	void ApplyCursorMode();
	void UpdateDisplayMode();
	void CreateDefaultCursor();
	void LoadWindowIcon(const std::string& path);
};
} // namespace Core
///////////////////////////////////////////////////////////////
