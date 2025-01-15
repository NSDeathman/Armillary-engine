///////////////////////////////////////////////////////////////
//Created: 15.01.2025
//Author: NS_Deathman
//Log realization
///////////////////////////////////////////////////////////////
#include "log.h"
///////////////////////////////////////////////////////////////
CLog::CLog()
{
#ifdef NDEBUG
	if (strstr((LPCSTR)GetCommandLine(), "-external_console_log"))
#endif
		CreateConsole();
}

void __cdecl CLog::Print(LPCSTR format, ...)
{
	va_list mark;
	string1024 buf;
	va_start(mark, format);
	int sz = _vsnprintf_s(buf, sizeof(buf) - 1, format, mark);
	buf[sizeof(buf) - 1] = 0;
	va_end(mark);
	if (sz)
	{
		std::cout << (buf);
		std::cout << "\n";
	}
}

void __cdecl CLog::Debug_Print(LPCSTR format, ...)
{
#ifdef _DEBUG
	va_list mark;
	string1024 buf;
	va_start(mark, format);
	int sz = _vsnprintf_s(buf, sizeof(buf) - 1, format, mark);
	buf[sizeof(buf) - 1] = 0;
	va_end(mark);
	if (sz)
	{
		std::cout << (buf);
		std::cout << "\n";
	}
#endif
}

void CLog::CreateConsole()
{
	int oldin;
	int oldout;

	if (AllocConsole())
	{
		(void)freopen("CONIN$", "r", stdin);
		(void)freopen("CONOUT$", "w", stderr);
		(void)freopen("CONOUT$", "w", stdout);

		oldin = GetConsoleCP();
		oldout = GetConsoleOutputCP();
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
	}

	_set_error_mode(_OUT_TO_STDERR);
}
///////////////////////////////////////////////////////////////
