///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Splash screen realization
///////////////////////////////////////////////////////////////
#include "splash_screen.h"
///////////////////////////////////////////////////////////////
#pragma warning(disable : 4100)
INT_PTR CALLBACK SplashScreenDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		DestroyWindow(hw);
		break;
	case WM_COMMAND:
		if (LOWORD(wp) == IDCANCEL)
			DestroyWindow(hw);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
#pragma warning(default : 4100)
///////////////////////////////////////////////////////////////
CSplashScreen::CSplashScreen()
{
	m_splash_screen_window = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STARTUP), 0, SplashScreenDlgProc);

	SetWindowPos(m_splash_screen_window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	ShowWindow(m_splash_screen_window, SW_SHOWDEFAULT);
	UpdateWindow(m_splash_screen_window);
}

CSplashScreen::~CSplashScreen()
{
	DestroyWindow(m_splash_screen_window);
	m_splash_screen_window = NULL;
}
///////////////////////////////////////////////////////////////
