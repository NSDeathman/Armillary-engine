///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Splash screen realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <windows.h>
///////////////////////////////////////////////////////////////
class CSplashScreen
{
  public:
	CSplashScreen();
	~CSplashScreen();

	void Show();
	void Hide();
	void UpdateProgress(const char* message = nullptr);

  private:
	HWND m_splash_screen_window;
	HBITMAP m_hBitmap;
	HWND m_progress_bar;
	HWND m_status_text;

	static INT_PTR CALLBACK SplashScreenDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void LoadBitmapFromResource();
	void CenterWindow();
};
///////////////////////////////////////////////////////////////
extern CSplashScreen* SplashScreen;
///////////////////////////////////////////////////////////////

