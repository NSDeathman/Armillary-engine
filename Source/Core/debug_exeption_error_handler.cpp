///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
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

// Инициализация системы обработки ошибок
void ErrorHandler::initialize(CrashCallback customCrashHandler)
{
	if (m_initialized)
		return;

	// Установка обработчиков
	if (customCrashHandler)
	{
		m_crashHandler = std::make_unique<CrashCallback>(customCrashHandler);
	}
	else
	{
		initializeDefaultCrashHandler();
	}

	// Установка обработчика ассертов по умолчанию
	m_assertionHandler = std::make_unique<std::function<void(const std::string&)>>(defaultAssertionHandler);

	// Установка обработчиков сигналов (опционально)
	try
	{
		std::signal(SIGSEGV, [](int signal) { handleCriticalError("Segmentation fault (SIGSEGV)"); });

		std::signal(SIGABRT, [](int signal) { handleCriticalError("Abort signal (SIGABRT)"); });

		std::signal(SIGFPE, [](int signal) { handleCriticalError("Floating point exception (SIGFPE)"); });
	}
	catch (...)
	{
		// Игнорируем ошибки при установке обработчиков сигналов
	}

	Core::Debug::StackWalker::getInstance().initialize();

	m_initialized = true;

	Log("Error handling system initialized");
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

	Debug::StackWalker::logStackTrace("Critical error additional context");

	if (m_crashHandler && *m_crashHandler)
	{
		try
		{
			(*m_crashHandler)(e);
		}
		catch (...)
		{
			ErrLog("Error in crash handler!");
		}
	}
	else
	{
		defaultCrashHandler(e);
	}

	if (m_terminateOnCritical)
	{
		std::terminate();
	}
}

void ErrorHandler::handleCriticalError(const std::exception& e)
{
	logStdException(e, Core::LogLevel::error);

	std::string Message = std::string("\nCritical: ") + std::string(e.what());
	Core::Debug::Exception wrappedException(Message);
	handleCriticalError(wrappedException);
}

void ErrorHandler::handleCriticalError(const std::string& message)
{
	std::string Message = std::string("\nCritical: ") + message;
	Core::Debug::Exception criticalException(Message);
	handleCriticalError(criticalException);
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
