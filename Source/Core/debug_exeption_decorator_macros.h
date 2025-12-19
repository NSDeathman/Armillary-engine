///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#define SAFE_CALL(function, ...) Core::Debug::ExceptionDecorator::SafeInvoke(function, ##__VA_ARGS__)

#define GRAPHICS_CALL(function, ...) Core::Debug::GraphicsDecorator::SafeModuleCall(function, ##__VA_ARGS__)

#define RESOURCE_CALL(function, ...) Core::Debug::ResourceDecorator::SafeModuleCall(function, ##__VA_ARGS__)

#define NETWORK_CALL(function, ...) Core::Debug::NetworkDecorator::SafeModuleCall(function, ##__VA_ARGS__)

#define SAFE_EXECUTE(operation) Core::Debug::ExceptionDecorator::SafeInvoke([&]() { operation; })
///////////////////////////////////////////////////////////////
