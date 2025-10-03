///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// Log realization
///////////////////////////////////////////////////////////////
#include "log.h"
#include "filesystem.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	CLog::CLog() : m_Initialized(false), m_ConsoleCreated(false)
	{
		// Конструктор вызывается только через GetInstance()
	}

	CLog::~CLog()
	{
		Destroy();
	}

	void CLog::Initialize(const std::string& logDir)
	{
		if (m_Initialized)
		{
			Print("Logger already initialized!");
			return;
		}

		try
		{
			// Создаем директорию для логов
			std::filesystem::create_directories(logDir);

			// Создаем путь к файлу лога
			m_LogFilePath = logDir + "/armillary_engine.log";

			// Очищаем файл и создаем поток
			m_LogFileStream.open(m_LogFilePath, std::ofstream::out | std::ofstream::trunc);
			m_LogFileStream.close();
			m_LogFileStream.open(m_LogFilePath, std::ios_base::out | std::ios_base::app);

			if (!m_LogFileStream.is_open())
			{
				throw std::runtime_error("Failed to open log file: " + m_LogFilePath);
			}

			m_Initialized = true;

			// Создаем консоль если нужно
	#ifdef NDEBUG
			if (strstr(GetCommandLineA(), "-external_console_log"))
	#endif
			{
				CreateConsole();
			}

			Print("Logger initialized successfully");
			Print("Log file: %s", m_LogFilePath.c_str());
		}
		catch (const std::exception& e)
		{
			// Fallback to console output if file logging fails
			AllocConsole();
			freopen("CONOUT$", "w", stdout);
			printf("Failed to initialize logger: %s\n", e.what());
		}
	}

	void CLog::Flush()
	{
		if (m_Initialized)
		{
			Print("Flushing log...");
			m_LogFileStream.flush();
		}
	}

	void CLog::Destroy()
	{
		if (m_Initialized)
		{
			Flush();
			Print("Destroying logger...");
			m_LogFileStream.close();
			m_Initialized = false;
		}
	}

	void CLog::SetConsoleColor(ConsoleColor color)
	{
		if (m_ConsoleCreated)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
		}
	}

	void CLog::ResetConsoleColor()
	{
		if (m_ConsoleCreated)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, static_cast<WORD>(GRAY)); // Возвращаем стандартный цвет
		}
	}

	std::string CLog::GetLevelString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::debug:   return "[DEBUG]";
		case LogLevel::info:    return "[INFO]";
		case LogLevel::warning: return "[WARNING]";
		case LogLevel::error:   return "[ERROR]";
		default:                return "[UNKNOWN]";
		}
	}

	CLog::ConsoleColor CLog::GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::debug:   return GRAY;        // Серый
		case LogLevel::info:    return WHITE;       // Белый
		case LogLevel::warning: return YELLOW;      // Желтый
		case LogLevel::error:   return RED;         // Красный
		default:                return WHITE;
		}
	}

	void CLog::PrintInternal(LPCSTR format, va_list args, LogLevel level, bool isDebug)
	{
		if (!m_Initialized && !isDebug)
		{
			// Fallback для случаев когда логгер еще не инициализирован
			char buffer[1024];
			vsprintf_s(buffer, format, args);
			OutputDebugStringA(buffer);
			OutputDebugStringA("\n");
			return;
		}

		char messageBuffer[1024];
		int sz = vsprintf_s(messageBuffer, sizeof(messageBuffer), format, args);

		if (sz > 0)
		{
			messageBuffer[sizeof(messageBuffer) - 1] = 0;

			// Формируем полное сообщение с временем и уровнем
			auto now = std::chrono::system_clock::now();
			auto time_t = std::chrono::system_clock::to_time_t(now);
			std::tm timeInfo;
			localtime_s(&timeInfo, &time_t);

			char timeBuffer[64];
			strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

			char fullBuffer[1200];
			sprintf_s(fullBuffer, sizeof(fullBuffer), "%s %s %s", 
					 timeBuffer, GetLevelString(level).c_str(), messageBuffer);

			// Вывод в консоль с цветом
			if (m_ConsoleCreated)
			{
				SetConsoleColor(GetLevelColor(level));
				std::cout << fullBuffer << std::endl;
				ResetConsoleColor();
			}
			else
			{
				// Если консоль не создана, выводим без цвета
				std::cout << fullBuffer << std::endl;
			}

			// Вывод в файл (если не debug сообщение или логгер инициализирован)
			if (m_Initialized && (!isDebug || m_LogFileStream.is_open()))
			{
				m_LogFileStream << fullBuffer << std::endl;
			}

			// Вывод в Debug Output (без цвета)
			OutputDebugStringA(fullBuffer);
			OutputDebugStringA("\n");
		}
	}

	void __cdecl CLog::Print(LPCSTR format, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInternal(format, args, LogLevel::info, false);
		va_end(args);
	}

	void __cdecl CLog::PrintWithLevel(LPCSTR format, LogLevel level, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInternal(format, args, level, false);
		va_end(args);
	}

	void __cdecl CLog::Debug_Print(LPCSTR format, ...)
	{
	#ifdef _DEBUG
		va_list args;
		va_start(args, format);
		PrintInternal(format, args, LogLevel::debug, true);
		va_end(args);
	#endif
	}

	void __cdecl CLog::Warning_Print(LPCSTR format, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInternal(format, args, LogLevel::warning, false);
		va_end(args);
	}

	void __cdecl CLog::Error_Print(LPCSTR format, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInternal(format, args, LogLevel::error, false);
		va_end(args);
	}

	void CLog::CreateConsole()
	{
		if (m_ConsoleCreated)
			return;

		if (AllocConsole())
		{
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stderr);
			freopen("CONOUT$", "w", stdout);

			SetConsoleCP(1251);
			SetConsoleOutputCP(1251);

			// Устанавливаем начальный цвет
			SetConsoleColor(GRAY);

			m_ConsoleCreated = true;
			Print("Console created successfully");
		}
		else
		{
			DWORD error = GetLastError();
			OutputDebugStringA(("Failed to create console, error: " + std::to_string(error)).c_str());
		}
	}
///////////////////////////////////////////////////////////////
} // namespace Core
///////////////////////////////////////////////////////////////
