///////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: NS_Deathman
// Application entry point
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
///////////////////////////////////////////////////////////////
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
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

	std::unique_ptr<CApplicationBase> Engine = std::make_unique<CEngine>();

	Engine->Process();

    return 0;
}
///////////////////////////////////////////////////////////////
