///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
#include "debug_exeption_error_handler.h"
#include <functional>
#include <type_traits>
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
class ExceptionDecorator
{
  public:
	// Основной метод для безопасного выполнения
	template <typename F, typename... Args>
	static auto SafeInvoke(F&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...))
	{

		using ReturnType = decltype(func(std::forward<Args>(args)...));

		try
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				func(std::forward<Args>(args)...);
			}
			else
			{
				return func(std::forward<Args>(args)...);
			}
		}
		catch (const Exception& e)
		{
			ErrorHandler::logException(e);
			throw;
		}
		catch (const std::exception& e)
		{
			ErrorHandler::logStdException(e);
			throw Core::Debug::Exception(std::string("Standard exception: ") + e.what());
		}
		catch (...)
		{
			ErrorHandler::logException(Core::Debug::Exception("Unknown exception"));
			throw Core::Debug::Exception("Unknown exception occurred");
		}
	}
};
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
