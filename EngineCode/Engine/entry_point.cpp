///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//Application entry point
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Application.h"
#include "splash_screen.h"
///////////////////////////////////////////////////////////////
CSplashScreen* SplashScreen = NULL;
CApplication* App = NULL;
HWND g_splash_screen_window = NULL;
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
///////////////////////////////////////////////////////////////
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ScreenWidth = (uint16_t)LOWORD(lParam); // Queue resize
		g_ScreenHeight = (uint16_t)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInstance);

	SDL_Init(SDL_INIT_EVERYTHING);

	App = new(CApplication);

    App->Process();

	SDL_Quit();

    return 0;
}
///////////////////////////////////////////////////////////////
