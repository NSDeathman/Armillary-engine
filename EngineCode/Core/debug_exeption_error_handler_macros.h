///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "debug_exeption_error_handler.h"
///////////////////////////////////////////////////////////////
// Макросы для ассертов (замена R_ASSERT)
#define ENGINE_ASSERT(condition, message)                                                                              \
	Core::Debug::ErrorHandler::assertHandler(condition, message, std::source_location::current())

#define ASSERT_GRAPHICS(condition, message)                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(condition))                                                                                              \
		{                                                                                                              \
			THROW_GRAPHICS(message);                                                                                   \
		}                                                                                                              \
	} while (false)

// Макросы для безопасного выполнения с логированием
#define SAFE_EXECUTE_LOG(operation, context)                                                                           \
	try                                                                                                                \
	{                                                                                                                  \
		operation;                                                                                                     \
	}                                                                                                                  \
	catch (const Core::Debug::Exception& e)                                                                        \
	{                                                                                                                  \
		Core::Debug::ErrorHandler::logException(e);                                                                \
		throw;                                                                                                         \
	}                                                                                                                  \
	catch (const std::exception& e)                                                                                    \
	{                                                                                                                  \
		Core::Debug::ErrorHandler::logStdException(e);                                                             \
		throw Core::Debug::Exception(std::string(context) + ": " + e.what());                                      \
	}

// Макрос для критических ошибок
#define CRITICAL_ERROR(message) Core::Debug::ErrorHandler::handleCriticalError(message)
///////////////////////////////////////////////////////////////
