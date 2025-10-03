///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "debug_exeption_base.h"
#include "debug_exeption_base_inline.h"
#include "debug_exeption_decorator.h"
#include "debug_exeption_macros.h"
#include "debug_exeption_base_types.h"
#include "debug_exeption_base_types_inline.h"
#include "debug_exeption_module_decorator.h"
#include "debug_exeption_error_handler.h"
#include "debug_exeption_decorator_macros.h"
#include "debug_exeption_error_handler_macros.h"
#include "debug_stack_walker.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
	class Exception;
	class GraphicsException;
	class ResourceException;
	class AudioException;
	class NetworkException;

	class CORE_API ErrorHandler;
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
