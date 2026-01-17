///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Modified: 27.12.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Window.h"
#include "log.h"
#include <algorithm>
///////////////////////////////////////////////////////////////

namespace Core
{
CWindow::CWindow() : m_window(nullptr), m_cursor(nullptr), m_initialized(false), m_windowID(0)
{
	SDL_zero(m_windowInfo);
	SDL_zero(m_displayMode);
}

CWindow::~CWindow()
{
	Destroy();
}

void CWindow::Initialize(const Config& config)
{
	if (m_initialized)
	{
		Print("Window already initialized");
		return;
	}

	m_config = config;
	Create();
	UpdateDisplayMode();
	ApplyCursorMode();
	m_initialized = true;

	Print("Window created: %s (%dx%d)", m_config.Name.c_str(), m_config.Width, m_config.Height);
}

void CWindow::Destroy()
{
	if (!m_initialized)
		return;

	if (m_cursor)
	{
		SDL_FreeCursor(m_cursor);
		m_cursor = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	m_initialized = false;
	Print("Window destroyed");
}

void CWindow::Create()
{
	uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS;

	if (m_config.AllowHighDPI)
		flags |= SDL_WINDOW_ALLOW_HIGHDPI;
	if (m_config.Resizable)
		flags |= SDL_WINDOW_RESIZABLE;
	if (m_config.AlwaysOnTop)
		flags |= SDL_WINDOW_ALWAYS_ON_TOP;

	m_window = SDL_CreateWindow(m_config.Name.c_str(), m_config.Position.x, m_config.Position.y, m_config.Width, m_config.Height, flags);

	if (!m_window)
	{
		Print("! Failed to create window: %s", SDL_GetError());
		return;
	}

	m_windowID = SDL_GetWindowID(m_window);
	ApplyWindowMode();

	SDL_VERSION(&m_windowInfo.version);
	if (!SDL_GetWindowWMInfo(m_window, &m_windowInfo))
	{
		Print("! Failed to get window info: %s", SDL_GetError());
	}
}

void CWindow::ApplyWindowMode()
{
	if (!m_window)
		return;

	int result = 0;
	switch (m_config.Mode)
	{
	case WindowMode::Windowed:
		result = SDL_SetWindowFullscreen(m_window, 0);
		SDL_SetWindowBordered(m_window, SDL_TRUE);
		SDL_SetWindowResizable(m_window, m_config.Resizable ? SDL_TRUE : SDL_FALSE);
		break;

	case WindowMode::Borderless:
		result = SDL_SetWindowFullscreen(m_window, 0);
		SDL_SetWindowBordered(m_window, SDL_FALSE);
		SDL_SetWindowResizable(m_window, SDL_FALSE);
		break;

	case WindowMode::Fullscreen:
		result = SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
		break;
	}

	if (result < 0)
	{
		Print("! Failed to set window mode: %s", SDL_GetError());
	}
}

void CWindow::ApplyCursorMode()
{
	if (!m_window)
		return;

	switch (m_config.Cursor)
	{
	case CursorMode::Normal:
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		break;

	case CursorMode::Hidden:
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		break;

	case CursorMode::Confined:
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab(m_window, SDL_TRUE);
		break;
	}
}

void CWindow::UpdateDisplayMode()
{
	if (!m_window)
		return;

	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	if (displayIndex < 0)
	{
		Print("! Failed to get window display index: %s", SDL_GetError());
		return;
	}

	if (SDL_GetCurrentDisplayMode(displayIndex, &m_displayMode) != 0)
	{
		Print("! Failed to get display mode: %s", SDL_GetError());
	}
}

// Основные операции окна
void CWindow::Show()
{
	if (m_window)
		SDL_ShowWindow(m_window);
}

void CWindow::Hide()
{
	if (m_window)
		SDL_HideWindow(m_window);
}

void CWindow::Minimize()
{
	if (m_window)
		SDL_MinimizeWindow(m_window);
}

void CWindow::Maximize()
{
	if (m_window)
		SDL_MaximizeWindow(m_window);
}

void CWindow::Restore()
{
	if (m_window)
		SDL_RestoreWindow(m_window);
}

void CWindow::Close()
{
	Destroy();
}

void CWindow::Focus()
{
	if (m_window)
		SDL_RaiseWindow(m_window);
}

bool CWindow::HasFocus() const
{
	return m_window && (SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS);
}

bool CWindow::IsMinimized() const
{
	return m_window && (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED);
}

bool CWindow::IsMaximized() const
{
	return m_window && (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MAXIMIZED);
}

bool CWindow::IsVisible() const
{
	return m_window && (SDL_GetWindowFlags(m_window) & SDL_WINDOW_SHOWN);
}

// Изменение свойств
void CWindow::SetMode(WindowMode mode)
{
	m_config.Mode = mode;
	ApplyWindowMode();
}

void CWindow::SetResolution(uint32_t width, uint32_t height)
{
	if (!m_window)
		return;

	m_config.Width = width;
	m_config.Height = height;
	SDL_SetWindowSize(m_window, width, height);
	Center();
}

void CWindow::SetPosition(int x, int y)
{
	if (!m_window)
		return;
	SDL_SetWindowPosition(m_window, x, y);
}

void CWindow::SetTitle(const std::string& title)
{
	if (!m_window)
		return;
	m_config.Name = title;
	SDL_SetWindowTitle(m_window, title.c_str());
}

void CWindow::SetIcon(const std::string& iconPath)
{
	if (!m_window)
		return;
	LoadWindowIcon(iconPath);
}

void CWindow::SetVSync(bool enabled)
{
	m_config.VSync = enabled;
}

void CWindow::SetResizable(bool resizable)
{
	m_config.Resizable = resizable;
	if (m_window)
		SDL_SetWindowResizable(m_window, resizable ? SDL_TRUE : SDL_FALSE);
}

void CWindow::SetAlwaysOnTop(bool alwaysOnTop)
{
	m_config.AlwaysOnTop = alwaysOnTop;
	if (m_window)
		SDL_SetWindowAlwaysOnTop(m_window, alwaysOnTop ? SDL_TRUE : SDL_FALSE);
}

void CWindow::SetCursorMode(CursorMode mode)
{
	m_config.Cursor = mode;
	ApplyCursorMode();
}

void CWindow::SetMouseCapture(bool capture)
{
	if (m_window)
		SDL_SetWindowGrab(m_window, capture ? SDL_TRUE : SDL_FALSE);
}

void CWindow::SetBrightness(float brightness)
{
	if (m_window)
		SDL_SetWindowBrightness(m_window, std::clamp(brightness, 0.0f, 1.0f));
}

void CWindow::SetOpacity(float opacity)
{
	if (m_window)
		SDL_SetWindowOpacity(m_window, std::clamp(opacity, 0.0f, 1.0f));
}

// Получение свойств
Math::float2 CWindow::GetSize() const
{
	int w = 0, h = 0;
	if (m_window)
		SDL_GetWindowSize(m_window, &w, &h);
	return Math::float2(w, h);
}

Math::float2 CWindow::GetPosition() const
{
	int x = 0, y = 0;
	if (m_window)
		SDL_GetWindowPosition(m_window, &x, &y);
	return Math::float2(x, y);
}

Math::float2 CWindow::GetDrawableSize() const
{
	int w = 0, h = 0;
	if (m_window)
		SDL_GL_GetDrawableSize(m_window, &w, &h);
	return Math::float2(w, h);
}

float CWindow::GetBrightness() const
{
	return m_window ? SDL_GetWindowBrightness(m_window) : 1.0f;
}

float CWindow::GetOpacity() const
{
	float opacity = 1.0f;
	if (m_window)
		SDL_GetWindowOpacity(m_window, &opacity);
	return opacity;
}

std::string CWindow::GetTitle() const
{
	return m_config.Name;
}

WindowMode CWindow::GetMode() const
{
	return m_config.Mode;
}

uint32_t CWindow::GetDisplayIndex() const
{
	return m_window ? SDL_GetWindowDisplayIndex(m_window) : 0;
}

HWND CWindow::GetWindowHandle() const
{
	return m_windowInfo.info.win.window;
}

SDL_Window* CWindow::GetSDLWindow() const
{
	return m_window;
}

SDL_DisplayMode CWindow::GetCurrentDisplayMode() const
{
	return m_displayMode;
}

std::vector<SDL_DisplayMode> CWindow::GetAvailableDisplayModes() const
{
	std::vector<SDL_DisplayMode> modes;
	int displayIndex = GetDisplayIndex();
	int count = SDL_GetNumDisplayModes(displayIndex);

	for (int i = 0; i < count; ++i)
	{
		SDL_DisplayMode mode;
		if (SDL_GetDisplayMode(displayIndex, i, &mode) == 0)
		{
			modes.push_back(mode);
		}
	}

	return modes;
}

void CWindow::Center()
{
	CenterOnDisplay(GetDisplayIndex());
}

void CWindow::CenterOnDisplay(uint32_t displayIndex)
{
	if (!m_window)
		return;

	SDL_Rect bounds;
	if (SDL_GetDisplayBounds(displayIndex, &bounds) == 0)
	{
		Math::float2 size = GetSize();
		int centerX = bounds.x + (bounds.w - size.x) / 2;
		int centerY = bounds.y + (bounds.h - size.y) / 2;
		SetPosition(centerX, centerY);
	}
}

void CWindow::SetEventCallback(const EventCallback& callback)
{
	m_eventCallback = callback;
}

void CWindow::ProcessEvents()
{
	if (!m_eventCallback)
		return;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		m_eventCallback(event);
	}
}

void CWindow::TakeScreenshot(const std::string& filepath)
{
}

void CWindow::Flash(bool flash)
{
#ifdef _WIN32
	if (m_window)
	{
		FlashWindow(GetWindowHandle(), flash);
	}
#endif
}

void CWindow::WarpMouse(int x, int y)
{
	if (m_window)
		SDL_WarpMouseInWindow(m_window, x, y);
}

Math::float2 CWindow::GetMousePosition() const
{
	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	return Math::float2(x, y);
}

void CWindow::Reset(const Config& config)
{
	Destroy();
	Initialize(config);
}

void CWindow::CreateDefaultCursor()
{
	if (m_cursor)
	{
		SDL_FreeCursor(m_cursor);
	}

	// Создание невидимого курсора
	Uint8 data[1] = {0};
	Uint8 mask[1] = {0};
	m_cursor = SDL_CreateCursor(data, mask, 8, 8, 0, 0);
}

void CWindow::LoadWindowIcon(const std::string& path)
{
#if 0
	SDL_Surface* icon = IMG_Load(path.c_str());
	if (icon)
	{
		SDL_SetWindowIcon(m_window, icon);
		SDL_FreeSurface(icon);
	}
	else
#endif
	{
		Print("! Failed to load icon: %s");//, IMG_GetError());
	}
}

} // namespace Core
  ///////////////////////////////////////////////////////////////
