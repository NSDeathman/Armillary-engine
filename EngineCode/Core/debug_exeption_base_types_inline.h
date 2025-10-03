///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
#include "debug_exeption_base_types.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
    inline GraphicsException::GraphicsException(const std::string& message, 
                                              std::source_location location)
        : Exception("Graphics: " + message, location) {}

    inline ResourceException::ResourceException(const std::string& message, 
                                              std::source_location location)
        : Exception("Resource: " + message, location) {}

    inline AudioException::AudioException(const std::string& message, 
                                        std::source_location location)
        : Exception("Audio: " + message, location) {}

    inline NetworkException::NetworkException(const std::string& message, 
                                            std::source_location location)
        : Exception("Network: " + message, location) {}
} // namespace Core::Debug
///////////////////////////////////////////////////////////////