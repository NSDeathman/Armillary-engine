///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// Log realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "architect_patterns.h"
#include <fstream>
#include "filesystem.h"
#include "windows.h"
///////////////////////////////////////////////////////////////
CORE_BEGIN
enum class LogLevel
{
	debug,
	info,
	warning,
	error,
	unknown
};

class CLog
{
	public:
	CLog();
	~CLog();

	void Initialize(const std::string& logDir = LOGS);
	void Flush();
	void Destroy();

	void CreateConsole();
	void __cdecl PrintInternal(LPCSTR format, ...);
	void __cdecl PrintInternalWithLevel(LPCSTR format, LogLevel level = LogLevel::error, ...);
	void __cdecl Debug_Print(LPCSTR format, ...);
	void __cdecl Warning_Print(LPCSTR format, ...);
	void __cdecl Error_Print(LPCSTR format, ...);

	bool IsInitialized() const
	{
		return m_Initialized;
	}
	const std::string& GetLogPath() const
	{
		return m_LogFilePath;
	}

	private:
	std::ofstream m_LogFileStream;
	std::string m_LogFilePath;
	bool m_Initialized = false;
	bool m_ConsoleCreated = false;

	// Цвета для консоли Windows
	enum ConsoleColor
	{
		BLACK = 0,
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_CYAN = 3,
		DARK_RED = 4,
		DARK_MAGENTA = 5,
		DARK_YELLOW = 6,
		GRAY = 7,
		DARK_GRAY = 8,
		BLUE = 9,
		GREEN = 10,
		CYAN = 11,
		RED = 12,
		MAGENTA = 13,
		YELLOW = 14,
		WHITE = 15
	};

	void PrintInternal(LPCSTR format, va_list args, LogLevel level, bool isDebug = false);
	void SetConsoleColor(ConsoleColor color);
	void ResetConsoleColor();
	std::string GetLevelString(LogLevel level);
	ConsoleColor GetLevelColor(LogLevel level);
};
CORE_END
///////////////////////////////////////////////////////////////
