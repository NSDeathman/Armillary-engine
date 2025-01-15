///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//Application entry point
///////////////////////////////////////////////////////////////
#include "Application.h"
#include "splash_screen.h"
///////////////////////////////////////////////////////////////
CSplashScreen* SplashScreen = NULL;
CApplication* App = NULL;
HWND g_splash_screen_window = NULL;
///////////////////////////////////////////////////////////////
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        App->Destroy();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static INT_PTR CALLBACK SplashScreenDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
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

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInstance);

	App = new(CApplication);

    App->Process();

    return 0;
}
///////////////////////////////////////////////////////////////
