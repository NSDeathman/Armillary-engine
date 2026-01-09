///////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: NS_Deathman
// Application entry point
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
///////////////////////////////////////////////////////////////
void GlobalSignalHandler(int signal)
{
	if (signal == SIGABRT)
		MessageBoxA(NULL, "Caught SIGABRT: Usually caused by runtime assertion.", "Error", MB_OK | MB_ICONERROR);
}

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
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		std::unique_ptr<CApplicationBase> Engine = std::make_unique<CEngine>();
		Engine->Process();
	}
	catch (const Core::Debug::Exception& e)
	{
		Core::Debug::ErrorHandler::handleCriticalError(e);
	}
	catch (const std::exception& e)
	{
		Core::Debug::ErrorHandler::handleCriticalError(e);
	}
	catch (...)
	{
		Core::Debug::ErrorHandler::handleCriticalError("Unknown unhandled exception");
	}

	return 0;
}
///////////////////////////////////////////////////////////////
