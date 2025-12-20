///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
#include "debug_exeption_base_types.h"
///////////////////////////////////////////////////////////////
// Основные макросы для бросания исключений
#define ENGINE_THROW(exception_type, message) \
    throw Core::Debug::exception_type(message, std::source_location::current())

// Специализированные макросы
#define THROW_GRAPHICS(message) ENGINE_THROW(GraphicsException, message)
#define THROW_RESOURCE(message) ENGINE_THROW(ResourceException, message)
#define THROW_AUDIO(message) ENGINE_THROW(AudioException, message)
#define THROW_NETWORK(message) ENGINE_THROW(NetworkException, message)
#define THROW_GAME(message) ENGINE_THROW(GameException, message)
#define THROW_ENGINE(message) ENGINE_THROW(EngineException, message)

// Макросы для проверки условий
#define ENGINE_CHECK(condition, exception_type, message) \
    do { \
        if (!(condition)) { \
            ENGINE_THROW(exception_type, message); \
        } \
    } while(false)

#define CHECK_GRAPHICS(condition, message) \
    ENGINE_CHECK(condition, GraphicsException, message)

#define CHECK_RESOURCE(condition, message) \
    ENGINE_CHECK(condition, ResourceException, message)

// Макрос для логирования исключений
#define LOG_EXCEPTION(except) \
    Engine::ErrorHandler::logException(except)
///////////////////////////////////////////////////////////////
