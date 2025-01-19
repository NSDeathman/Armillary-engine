///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#include "main_window.h"
#include "log.h"
///////////////////////////////////////////////////////////////
CMainWindow::CMainWindow()
{
	Msg("Creating window...");

	SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | 
												 SDL_WINDOW_INPUT_FOCUS |
												 SDL_WINDOW_BORDERLESS);

	m_window = SDL_CreateWindow("Armillary engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, winFlags);

	SDL_VERSION(&m_WindowInfo.version);

	HRESULT result = SDL_GetWindowWMInfo(m_window, &m_WindowInfo);

	if (result == E_FAIL)
	{
		Msg("! Failed to retrieve SDL window handle: %s", SDL_GetError());
		return;
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

CMainWindow::~CMainWindow()
{
	SDL_DestroyWindow(m_window);
}
///////////////////////////////////////////////////////////////
