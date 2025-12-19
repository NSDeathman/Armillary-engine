///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
#include "debug_exeption_base_types.h"
#include "debug_exeption_base_types_inline.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
template<typename ExceptionType>
class ModuleDecorator {
public:
    template<typename F, typename... Args>
    static auto SafeModuleCall(F&& func, Args&&... args) 
        -> decltype(func(std::forward<Args>(args)...)) {
        
        try {
            return ExceptionDecorator::SafeInvoke(std::forward<F>(func), 
                                                std::forward<Args>(args)...);
        }
        catch (const Exception& e) {
            throw ExceptionType(std::string("Module operation failed: ") + e.what());
        }
    }
};

// Специализации для конкретных модулей
using GraphicsDecorator = ModuleDecorator<GraphicsException>;
using ResourceDecorator = ModuleDecorator<ResourceException>;
using AudioDecorator = ModuleDecorator<AudioException>;
using NetworkDecorator = ModuleDecorator<NetworkException>;
} // namespace Core::Debug
///////////////////////////////////////////////////////////////