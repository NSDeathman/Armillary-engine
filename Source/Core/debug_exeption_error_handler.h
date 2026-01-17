///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
#include "Log.h"
#include <string>
#include <functional>
#include <memory>
#include "debug_exeption_base.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
class ErrorHandler
{
  public:
	// “ип функции дл€ обработки критических ошибок
	using CrashCallback = std::function<void(const Exception&)>;

	// »нициализаци€ системы обработки ошибок
	static void initialize(CrashCallback customCrashHandler = nullptr);
	static void shutdown();

	// ќсновные методы логировани€ исключений
	static void logException(const Exception& e, Core::LogLevel level = Core::LogLevel::error);
	static void logStdException(const std::exception& e, Core::LogLevel level = Core::LogLevel::error);
	static void logUnknownException(Core::LogLevel level = Core::LogLevel::error);

	// ќбработка критических ошибок (вызывают terminate)
	static void handleCriticalError(const Exception& e);
	static void handleCriticalError(const std::exception& e);
	static void handleCriticalError(const std::string& message);

	// ”становка пользовательских обработчиков
	static void setCrashHandler(CrashCallback handler);
	static void setAssertionHandler(std::function<void(const std::string&)> handler);

	// ¬спомогательные методы
	static bool hasInitialized()
	{
		return m_initialized;
	}
	static void setTerminateOnCritical(bool terminate)
	{
		m_terminateOnCritical = terminate;
	}

	// ћетод дл€ обработки ассертов (замена R_ASSERT)
	static void assertHandler(bool condition, const std::string& message,
							  std::source_location location = std::source_location::current());

  private:
	// ќЅя«ј“≈Ћ№Ќќ: объ€вл€ем статические переменные
	static bool m_initialized;
	static bool m_terminateOnCritical;
	static std::unique_ptr<CrashCallback> m_crashHandler;
	static std::unique_ptr<std::function<void(const std::string&)>> m_assertionHandler;

	static void initializeDefaultCrashHandler();
	static void defaultCrashHandler(const Exception& e);
	static void defaultAssertionHandler(const std::string& message);

	static std::string formatAssertionMessage(const std::string& message, const std::source_location& location);
};
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
