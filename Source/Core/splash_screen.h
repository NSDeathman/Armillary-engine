///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Splash screen realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <windows.h>
#include "Core.h"
#include "architect_patterns.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	class CORE_API CSplashScreen : public Patterns::Singleton<CSplashScreen>
	{
		friend class Patterns::Singleton<CSplashScreen>;

	  public:
		CSplashScreen();
		~CSplashScreen();

		void Show();
		void Hide();

	  private:
		HWND m_splash_screen_window;
		HBITMAP m_hBitmap;

		static INT_PTR CALLBACK SplashScreenDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void LoadBitmapFromResource();
		void CenterWindow();
	};
} // namespace Core
///////////////////////////////////////////////////////////////

