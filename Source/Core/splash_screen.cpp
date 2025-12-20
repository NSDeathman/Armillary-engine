///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: DeepSeek, NS_Deathman
// Splash screen realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <commctrl.h>
#include "splash_screen.h"
#include "..\Engine\resource.h"
///////////////////////////////////////////////////////////////
#pragma comment(lib, "comctl32.lib")
///////////////////////////////////////////////////////////////
namespace Core
{
	CSplashScreen::CSplashScreen(): m_splash_screen_window(NULL), m_hBitmap(NULL)
	{
		// Инициализация common controls
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_PROGRESS_CLASS;
		InitCommonControlsEx(&icex);

		// Создаем диалоговое окно
		m_splash_screen_window = CreateDialog(GetModuleHandle(NULL), 
											  MAKEINTRESOURCE(IDD_SPLASH), 
											  NULL,
											  SplashScreenDlgProc);

		if (m_splash_screen_window)
		{
			SetProcessDPIAware();
			SetWindowLongPtr(m_splash_screen_window, GWLP_USERDATA, (LONG_PTR)this);
			LoadBitmapFromResource();
			CenterWindow();
		}
	}

	CSplashScreen::~CSplashScreen()
	{
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}

		if (m_splash_screen_window)
		{
			DestroyWindow(m_splash_screen_window);
			m_splash_screen_window = NULL;
		}
	}

	void CSplashScreen::Show()
	{
		if (m_splash_screen_window)
		{
			SetWindowPos(m_splash_screen_window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			ShowWindow(m_splash_screen_window, SW_SHOW);
			UpdateWindow(m_splash_screen_window);
		}
	}

	void CSplashScreen::Hide()
	{
		if (m_splash_screen_window)
			ShowWindow(m_splash_screen_window, SW_HIDE);
	}

	void CSplashScreen::LoadBitmapFromResource()
	{
		// Загружаем BMP из ресурсов
		m_hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SPLASH_BMP));
	}

	void CSplashScreen::CenterWindow()
	{
		if (!m_splash_screen_window)
			return;

		RECT rcDialog, rcDesktop;
		GetWindowRect(m_splash_screen_window, &rcDialog);
		GetWindowRect(GetDesktopWindow(), &rcDesktop);

		int x = (rcDesktop.right - (rcDialog.right - rcDialog.left)) / 2;
		int y = (rcDesktop.bottom - (rcDialog.bottom - rcDialog.top)) / 2;

		SetWindowPos(m_splash_screen_window, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	INT_PTR CALLBACK CSplashScreen::SplashScreenDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		CSplashScreen* pThis = (CSplashScreen*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		switch (msg)
		{
		case WM_INITDIALOG: {
			pThis = (CSplashScreen*)lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			if (pThis)
			{
				if (pThis->m_hBitmap)
				{
					// Устанавливаем bitmap для static control
					HWND hImage = GetDlgItem(hwnd, IDC_STATIC);
					SendMessage(hImage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)pThis->m_hBitmap);
				}
			}
			return TRUE;
		}

		case WM_CTLCOLORDLG:
			return (INT_PTR)GetStockObject(WHITE_BRUSH);

		case WM_PAINT: {
			if (pThis && pThis->m_hBitmap)
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, pThis->m_hBitmap);

				BITMAP bmp;
				GetObject(pThis->m_hBitmap, sizeof(BITMAP), &bmp);

				BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

				SelectObject(hdcMem, hOldBitmap);
				DeleteDC(hdcMem);

				EndPaint(hwnd, &ps);
			}
			return TRUE;
		}

		case WM_ERASEBKGND:
			return TRUE; // Предотвращаем стандартную очистку фона

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return TRUE;

		case WM_DESTROY:
			PostQuitMessage(0);
			return TRUE;
		}
		return FALSE;
	}
} // namespace Core
///////////////////////////////////////////////////////////////
