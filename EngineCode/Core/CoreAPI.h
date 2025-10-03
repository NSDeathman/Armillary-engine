///////////////////////////////////////////////////////////////
// Created: 20.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
//  (\ /)
//  ( . .)
// c(")(")
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
///////////////////////////////////////////////////////////////
// |____|____|____|____|____|____|____|____|____|____|____|__//
// ____|____|____|____|____|____|____|____|____|____|____|___//
// __|____|____|____|____|___|_         ____|____|____|____|_//
// |____|____|____|____|___|    (\.-./)  _|____|____|____|___//
//____|____|____|____|____|_  = (^ Y ^) =  _|____|____|____|_//
//|____|____|____|____|____|___ /`---`\ __|____|____|____|___//
// __|____|____|____|____|____|_U___|_U|____|____|____|____|_//
// |____|____|____|____|____|____|____|____|____|____|____|__//
//_jgs|____|____|____|____|____|____|____|____|____|____|____//
///////////////////////////////////////////////////////////////
#include "splash_screen.h"
#include "window_implementation.h"
#include "TimeSystem.h"
#include "log.h"
#include "filesystem.h"
#include "build_identificator.h"
#include "cpu_identificator.h"
#include "AsyncExecutor.h"
#include "Input.h"
///////////////////////////////////////////////////////////////
// List of things whats we will exports
// from Core.dll to other engine components
///////////////////////////////////////////////////////////////
namespace Core
{
	// Classes
	class CORE_API CSplashScreen;
	class CORE_API CWindow;
	class CORE_API CTimeSystem;
	class CORE_API CLog;
	class CORE_API CAsyncExecutor;
	class CORE_API CInput;
	class CORE_API CFilesystem;

	// Functions
	CORE_API void PrintBuildData();
	CORE_API void InitializeCPU();
} // namespace Core
///////////////////////////////////////////////////////////////
