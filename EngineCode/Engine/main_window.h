///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <SDL/SDL_syswm.h>
///////////////////////////////////////////////////////////////
class CMainWindow
{
  public:
	SDL_Window* m_window;
	SDL_SysWMinfo m_WindowInfo;
	SDL_DisplayMode m_displayMode;
	SDL_Cursor* m_cursor;

  public:
	CMainWindow();
	~CMainWindow();

	void CreateSDLWindow();
	void DestroySDLWindow();

	void CenterWindow();

	HWND GetWindow();
	SDL_Window* GetSDLWindow();
	SDL_DisplayMode GetSDLDisplayMode();
	void CreateCursor();
	void HideCursor();
	void ShowCursor();
	void Reset();
	void ChangeScreenResolution();
};
///////////////////////////////////////////////////////////////
extern CMainWindow* MainWindow;
///////////////////////////////////////////////////////////////
