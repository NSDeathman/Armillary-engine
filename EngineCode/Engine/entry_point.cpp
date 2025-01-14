///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//Application entry point
///////////////////////////////////////////////////////////////
#include "Application.h"
///////////////////////////////////////////////////////////////
CApplication* App = NULL;
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

INT WINAPI main(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
    UNREFERENCED_PARAMETER(hInst);

	App = new(CApplication);

    App->Start();

	App->EventLoop();

    App->Destroy();

    //UnregisterClass(&wndclassname, wc.hInstance);
    return 0;
}
///////////////////////////////////////////////////////////////
