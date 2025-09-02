///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#include "main_window.h"
#include "log.h"
#include "EngineSettings.h"
#include "render_DX9.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
SDL_WindowFlags g_WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | 
												  SDL_WINDOW_INPUT_FOCUS | 
												  SDL_WINDOW_BORDERLESS | 
												  SDL_WINDOW_MAXIMIZED |
												  SDL_WINDOW_ALLOW_HIGHDPI);
///////////////////////////////////////////////////////////////
CMainWindow::CMainWindow()
{
	CreateSDLWindow();
	CreateCursor();
}

CMainWindow::~CMainWindow()
{
	DestroySDLWindow();
}

void CMainWindow::CreateSDLWindow()
{
	Msg("Creating window...");

	m_window = SDL_CreateWindow("Armillary engine", 
								SDL_WINDOWPOS_CENTERED, 
								SDL_WINDOWPOS_CENTERED, 
								g_ScreenWidth,
								g_ScreenHeight, 
								g_WindowFlags);

	SDL_VERSION(&m_WindowInfo.version);

	HRESULT result = SDL_GetWindowWMInfo(m_window, &m_WindowInfo);

	if (result == E_FAIL)
	{
		Msg("! Failed to retrieve SDL window handle: %s", SDL_GetError());
		return;
	}

	// Get the current display mode
	result = SDL_GetCurrentDisplayMode(0, &m_displayMode);

	if (result == E_FAIL)
	{
		Msg("! Failed to get SDL display mode: %s", SDL_GetError());
		return;
	}
}

void CMainWindow::CenterWindow()
{
	// Check if the window is valid
	if (!m_window)
	{
		Msg("! Cannot center window: window is not created.");
		return;
	}

	// Get the window size
	int windowWidth, windowHeight;
	SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

	// Calculate center position
	int centerX = (m_displayMode.w - windowWidth) / 2;
	int centerY = (m_displayMode.h - windowHeight) / 2;

	// Set the window position
	SDL_SetWindowPosition(m_window, centerX, centerY);

	DbgMsg("Window centered at %d, %d", centerX, centerY);
}

void CMainWindow::DestroySDLWindow()
{
	Msg("Destroying window...");

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

HWND CMainWindow::GetWindow()
{
	return m_WindowInfo.info.win.window;
}

SDL_Window* CMainWindow::GetSDLWindow()
{
	return m_window;
}

SDL_DisplayMode CMainWindow::GetSDLDisplayMode()
{
	return m_displayMode;
}

void CMainWindow::CreateCursor()
{
	Uint8 l_data[1];
	Uint8 l_mask[1];

	l_data[0] = 0;
	l_mask[0] = 0;

	m_cursor = SDL_CreateCursor(l_data, l_mask, 1, 1, 0, 0);
	SDL_SetCursor(m_cursor);
}

void CMainWindow::HideCursor()
{
	SDL_ShowCursor(false);
}

void CMainWindow::ShowCursor()
{
	SDL_ShowCursor(true);
}
///////////////////////////////////////////////////////////////
