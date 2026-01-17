///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Log.h"
#include <stdexcept>
#include <string>
#include <source_location>
///////////////////////////////////////////////////////////////
#include "debug_stack_walker.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
class Exception : public std::runtime_error
{
  public:
	explicit Exception(const std::string& message, std::source_location location = std::source_location::current())
		: std::runtime_error(formatMessage(message, location)), m_location(location)
	{
	}

	virtual ~Exception() = default;

	// Виртуальные методы для переопределения в производных классах
	virtual std::string getFullMessage() const
	{
		std::ostringstream oss;
		oss << "[" << getType() << "] " << what() << "Location: " << getLocationString();
		return oss.str();
	}

	virtual std::string getType() const
	{
		return "Exception";
	}

	virtual std::string getLocationString() const
	{
		std::ostringstream oss;
		oss << m_location.file_name() << ":" << m_location.line();
		if (m_location.function_name() && m_location.function_name()[0] != '\0')
		{
			oss << " in " << m_location.function_name();
		}
		return oss.str();
	}

	// Невиртуальные методы
	const std::source_location& getLocation() const noexcept
	{
		return m_location;
	}
	const std::string& getStackTrace() const noexcept
	{
		return m_stackTrace;
	}
	const std::string& getDetails() const noexcept
	{
		return m_details;
	}
	int getErrorCode() const noexcept
	{
		return m_errorCode;
	}
	const std::string& getContext() const noexcept
	{
		return m_context;
	}

	void setStackTrace(const std::string& stackTrace)
	{
		m_stackTrace = stackTrace;
	}
	void setContext(const std::string& context)
	{
		m_context = context;
	}
	void setErrorCode(int code)
	{
		m_errorCode = code;
	}
	void setDetails(const std::string& details)
	{
		m_details = details;
	}

  protected:
	std::source_location m_location;
	std::string m_stackTrace;
	std::string m_details;
	std::string m_context;
	int m_errorCode = 0;

	static std::string formatMessage(const std::string& message, const std::source_location& location)
	{
		return "[" + std::string(location.file_name()) + ":" + std::to_string(location.line()) + "]" + message;
	}
};
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
