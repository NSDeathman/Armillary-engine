///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
    // Базовые типы исключений
    class GraphicsException : public Exception {
    public:
        using Exception::Exception;
        GraphicsException(const std::string& message,
                         std::source_location location = std::source_location::current());
    };

    class ResourceException : public Exception {
    public:
        using Exception::Exception;
        ResourceException(const std::string& message,
                         std::source_location location = std::source_location::current());
    };

    class AudioException : public Exception {
    public:
        using Exception::Exception;
        AudioException(const std::string& message,
                      std::source_location location = std::source_location::current());
    };

    class NetworkException : public Exception {
    public:
        using Exception::Exception;
        NetworkException(const std::string& message,
                        std::source_location location = std::source_location::current());
    };
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
