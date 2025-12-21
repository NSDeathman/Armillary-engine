///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <eh.h>
#include "Log.h"
#include "filesystem.h"
#include "debug_stack_walker.h"
#include "debug_exeption_error_handler.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
bool ErrorHandler::m_initialized = false;
bool ErrorHandler::m_terminateOnCritical = true;
std::unique_ptr<ErrorHandler::CrashCallback> ErrorHandler::m_crashHandler = nullptr;
std::unique_ptr<std::function<void(const std::string&)>> ErrorHandler::m_assertionHandler = nullptr;

void SEHTranslator(unsigned int u, _EXCEPTION_POINTERS* pExp)
{
	// 1. Формируем сообщение об ошибке
	std::string error = "CRITICAL HARDWARE ERROR: 0x" + std::to_string(u);
	switch (u)
	{
	case 0xC0000005:
		error = "ACCESS VIOLATION (Invalid Memory Read/Write)";
		break;
	case 0xC00000FD:
		error = "STACK OVERFLOW";
		break;
	case 0xC0000094:
		error = "INTEGER DIVIDE BY ZERO";
		break;
	}

	std::cerr << "\n!!! " << error << " !!!\n";

	// 2. Получаем стек (используя PCONTEXT из исключения, это важно!)
	PCONTEXT ctx = pExp->ContextRecord;
	std::string stackTrace = "Failed to capture stack trace.";

	try
	{
		// Используем уже исправленный StackWalker
		stackTrace = Core::Debug::StackWalker::getInstance().getStackTraceString(64, 0, ctx);
	}
	catch (...)
	{
	}

	// 3. ПИШЕМ В ЛОГ И СОХРАНЯЕМ СРАЗУ
	ErrLog("================ CRASH REPORT ================");
	ErrLog("%s", error.c_str());
	ErrLog("----------------------------------------------");
	ErrLog("%s", stackTrace.c_str());
	ErrLog("==============================================");

	// 4. Создаем минидамп памяти (это полезнее лога, его можно открыть в VS)
#if defined(_WIN32) && defined(_DEBUG)
	// Эмуляция объекта Exception для вызова crash handler'а (если нужно)
	// Но лучше написать дамп прямо здесь, так как память может быть испорчена

	std::string dumpDir = std::string(DEBUG_DATA) + "memory_dmps/";
	std::filesystem::create_directories(dumpDir);
	std::string dumpPath = dumpDir + "CRASH_" + std::to_string(GetTickCount()) + ".dmp";

	HANDLE hFile = CreateFileA(dumpPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pExp; // <-- Самое важное: передаем реальный контекст сбоя
		mdei.ClientPointers = FALSE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, (pExp ? &mdei : nullptr),
						  nullptr, nullptr);
		CloseHandle(hFile);

		std::cerr << "Minidump saved to: " << dumpPath << std::endl;
	}
#endif

	Core::CLog::GetInstance().Flush();

	Sleep(100); 

	// 5. Убиваем процесс
	// Не используем throw, так как стек может быть поврежден.
	// Показываем сообщение и выходим.
	MessageBoxA(NULL, (error + "\n\nSee console/log for stack trace.").c_str(), "Armillary Engine Crash",
				MB_OK | MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), u);
}

// Инициализация системы обработки ошибок
void ErrorHandler::initialize(CrashCallback customCrashHandler)
{
	if (m_initialized)
		return;

	// 1. Установка транслятора SEH (превращает крэши в try-catch)
	_set_se_translator(SEHTranslator);

	// 2. Установка обработчиков
	if (customCrashHandler)
	{
		m_crashHandler = std::make_unique<CrashCallback>(customCrashHandler);
	}
	else
	{
		initializeDefaultCrashHandler();
	}

	m_assertionHandler = std::make_unique<std::function<void(const std::string&)>>(defaultAssertionHandler);

	// 3. Стандартные сигналы (на всякий случай)
	signal(SIGSEGV, [](int) { throw Core::Debug::Exception("Segmentation Fault (SIGSEGV)"); });
	signal(SIGABRT, [](int) { throw Core::Debug::Exception("Abort (SIGABRT)"); });
	signal(SIGFPE, [](int) { throw Core::Debug::Exception("Floating Point Exception"); });

	Core::Debug::StackWalker::getInstance().initialize();

	m_initialized = true;
	Log("Error handling system initialized (SEH Enabled)");
}

// Завершение работы
void ErrorHandler::shutdown()
{
	if (!m_initialized)
	{
		return;
	}

	Log("Error handling system shutdown");

	m_crashHandler.reset();
	m_assertionHandler.reset();
	m_initialized = false;
}

// Логирование исключений движка
void ErrorHandler::logException(const Exception& e, Core::LogLevel level)
{
	if (!m_initialized)
	{
		ErrLog("Exception: %s", e.what());
		ErrLog("Stack trace:\n");
		ErrLog("%s", e.getStackTrace());
		return;
	}

	ErrLog("Exception: %s", e.what());
	ErrLog("Location: %s", e.getLocationString().c_str());

	if (!e.getStackTrace().empty())
	{
		Log2("Stack trace: %s\n", level, e.getStackTrace().c_str());
	}
}

void ErrorHandler::logStdException(const std::exception& e, Core::LogLevel level)
{
	if (!m_initialized)
	{
		std::cerr << "\nStd exception: " << e.what() << std::endl;
		return;
	}

	std::ostringstream oss;
	oss << "\nStandard exception: " << e.what();

	Log2(oss.str().c_str(), level);
}

void ErrorHandler::logUnknownException(Core::LogLevel level)
{
	if (!m_initialized)
	{
		std::cerr << "\n[ERROR] Unknown exception occurred" << std::endl;
		return;
	}

	Log2("\nUnknown exception occurred", level);
}

void ErrorHandler::handleCriticalError(const Exception& e)
{
	logException(e, Core::LogLevel::error);

	if (e.getStackTrace().empty())
	{
		Debug::StackWalker::logStackTrace("Critical error context");
	}
	else
	{
		ErrLog("Captured Stack Trace:\n%s", e.getStackTrace().c_str());
	}

	Debug::StackWalker::logStackTrace("CRITICAL ERROR CONTEXT");

	if (m_crashHandler && *m_crashHandler)
		(*m_crashHandler)(e);
	else
		defaultCrashHandler(e);

	if (m_terminateOnCritical)
		std::terminate();
}

void ErrorHandler::handleCriticalError(const std::exception& e)
{
	handleCriticalError(Core::Debug::Exception(e.what()));
}

void ErrorHandler::handleCriticalError(const std::string& message)
{
	handleCriticalError(Core::Debug::Exception(message));
}

void ErrorHandler::setCrashHandler(CrashCallback handler)
{
	m_crashHandler = std::make_unique<CrashCallback>(handler);
}

void ErrorHandler::setAssertionHandler(std::function<void(const std::string&)> handler)
{
	m_assertionHandler = std::make_unique<std::function<void(const std::string&)>>(handler);
}

void ErrorHandler::assertHandler(bool condition, const std::string& message, std::source_location location)
{
	if (condition)
	{
		return;
	}

	std::string formattedMessage = formatAssertionMessage(message, location);

	if (m_assertionHandler && *m_assertionHandler)
	{
		try
		{
			(*m_assertionHandler)(formattedMessage);
		}
		catch (...)
		{
			defaultAssertionHandler(formattedMessage);
		}
	}
	else
	{
		defaultAssertionHandler(formattedMessage);
	}
}

void ErrorHandler::initializeDefaultCrashHandler()
{
	m_crashHandler = std::make_unique<CrashCallback>(defaultCrashHandler);
}

void ErrorHandler::defaultCrashHandler(const Exception& e)
{
	std::cerr << "\n=== CRITICAL ENGINE ERROR ===" << std::endl;
	std::cerr << "Message: " << e.what() << std::endl;
	std::cerr << "Location: " << e.getLocation().file_name() << ":" << e.getLocation().line() << std::endl;
	std::cerr << "=============================" << std::endl;

#if defined(_WIN32) && defined(_DEBUG)
	std::string filename = "engine_crash.dmp";
	std::string directory = std::string(DEBUG_DATA) + "memory_dmps//";
	std::filesystem::create_directories(directory);
	std::string fullpath = directory + filename;

	HANDLE hFile = CreateFileA(fullpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = nullptr;
		mdei.ClientPointers = FALSE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, nullptr, nullptr, nullptr);

		CloseHandle(hFile);
		Log("Crash dump saved to: %s", fullpath.c_str());
	}
#endif
}

void ErrorHandler::defaultAssertionHandler(const std::string& message)
{
	std::cerr << "[ASSERTION FAILED] " << message << std::endl;

#ifdef _DEBUG
	std::cerr << "Press Enter to continue..." << std::endl;
	std::cin.get();
#endif
}

std::string ErrorHandler::formatAssertionMessage(const std::string& message, const std::source_location& location)
{
	std::ostringstream oss;
	oss << "Assertion failed: " << message << " [File: " << location.file_name() << ", Line: " << location.line()
		<< ", Function: " << location.function_name() << "]";
	return oss.str();
}
} // namespace Core::Debug
  ///////////////////////////////////////////////////////////////
