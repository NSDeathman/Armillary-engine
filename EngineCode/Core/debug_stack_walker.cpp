///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Log.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "debug_stack_walker.h"
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
	StackWalker& StackWalker::getInstance()
	{
		static StackWalker instance;
		return instance;
	}

	bool StackWalker::initialize()
	{
		if (m_initialized)
		{
			return true;
		}

	#ifdef _WIN32
		m_process = GetCurrentProcess();

		// Инициализация символьной системы
		DWORD symOptions = SymGetOptions();
		symOptions |= SYMOPT_LOAD_LINES;
		symOptions |= SYMOPT_UNDNAME;
		symOptions |= SYMOPT_DEFERRED_LOADS;
		symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
		SymSetOptions(symOptions);

		if (!SymInitialize(m_process, nullptr, TRUE))
		{
			DWORD error = GetLastError();
			std::cerr << "Failed to initialize symbol handler. Error: " << error << std::endl;

			// Попробуем без путей символов
			if (!SymInitialize(m_process, nullptr, FALSE))
			{
				std::cerr << "Failed to initialize symbol handler even without symbols. Error: " << GetLastError()
						  << std::endl;
				return false;
			}
		}

		m_initialized = true;
		Log("StackWalker initialized");
	#endif

		return m_initialized;
	}

	void StackWalker::shutdown()
	{
		if (!m_initialized)
		{
			return;
		}

	#ifdef _WIN32
		SymCleanup(m_process);
	#endif

		m_initialized = false;
		Log("StackWalker shutdown");
	}

	std::vector<StackFrame> StackWalker::captureStackTrace(int maxFrames, int skipFrames)
	{
		std::vector<StackFrame> frames;

		if (!m_initialized)
		{
			StackFrame frame;
			frame.functionName = "StackWalker not initialized";
			frames.push_back(frame);
			return frames;
		}

	#ifdef _WIN32
		HANDLE thread = GetCurrentThread();

		// Получаем контекст
		CONTEXT context = {};
		RtlCaptureContext(&context);

		// Инициализируем frame
		STACKFRAME64 stackFrame = {};
		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Rbp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Rsp;
		stackFrame.AddrStack.Mode = AddrModeFlat;

		DWORD machineType = IMAGE_FILE_MACHINE_AMD64;

		// Настраиваем для разных архитектур
	#if defined(_M_IX86)
		machineType = IMAGE_FILE_MACHINE_I386;
		stackFrame.AddrPC.Offset = context.Eip;
		stackFrame.AddrFrame.Offset = context.Ebp;
		stackFrame.AddrStack.Offset = context.Esp;
	#elif defined(_M_AMD64)
		machineType = IMAGE_FILE_MACHINE_AMD64;
		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrFrame.Offset = context.Rbp;
		stackFrame.AddrStack.Offset = context.Rsp;
	#elif defined(_M_ARM64)
		machineType = IMAGE_FILE_MACHINE_ARM64;
		stackFrame.AddrPC.Offset = context.Pc;
		stackFrame.AddrFrame.Offset = context.Fp;
		stackFrame.AddrStack.Offset = context.Sp;
	#endif

		int frameCount = 0;
		int skipped = 0;

		while (frameCount < maxFrames)
		{
			BOOL result = StackWalk64(machineType, m_process, thread, &stackFrame, &context, nullptr,
									  SymFunctionTableAccess64, SymGetModuleBase64, nullptr);

			if (!result || stackFrame.AddrPC.Offset == 0)
			{
				break;
			}

			// Пропускаем кадры если нужно
			if (skipped < skipFrames)
			{
				skipped++;
				continue;
			}

			StackFrame frame;
			frame.address = toHexString(stackFrame.AddrPC.Offset);

			// Пытаемся получить имя функции
			frame.functionName = resolveAddress(stackFrame.AddrPC.Offset);
			frame.moduleName = getModuleName(stackFrame.AddrPC.Offset);

			// Пытаемся получить информацию о строке
			DWORD displacement = 0;
			IMAGEHLP_LINE64 lineInfo = {};
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (SymGetLineFromAddr64(m_process, stackFrame.AddrPC.Offset, &displacement, &lineInfo))
			{
				frame.fileName = lineInfo.FileName;
				frame.lineNumber = std::to_string(lineInfo.LineNumber);

				// Укорачиваем путь к файлу для читаемости
				size_t lastSlash = frame.fileName.find_last_of("\\/");
				if (lastSlash != std::string::npos)
				{
					frame.fileName = frame.fileName.substr(lastSlash + 1);
				}
			}

			frames.push_back(frame);
			frameCount++;
		}

	#else
		StackFrame frame;
		frame.functionName = "Stack walking not supported on this platform";
		frames.push_back(frame);
	#endif

		return frames;
	}

	std::string StackWalker::getStackTraceString(int maxFrames, int skipFrames)
	{
		auto frames = captureStackTrace(maxFrames, skipFrames + 1); // +1 чтобы пропустить этот вызов

		std::ostringstream oss;
		oss << "Stack trace (" << frames.size() << " frames):" << std::endl;

		for (size_t i = 0; i < frames.size(); ++i)
		{
			oss << "#" << i << " " << frames[i].toString() << std::endl;
		}

		return oss.str();
	}

	void StackWalker::logStackTrace(const std::string& context)
	{
		auto stackTrace = getInstance().getStackTraceString(32, 2); // Пропускаем 2 кадра

		if (!context.empty())
		{
			ErrLog("Stack trace for: %s", context.c_str());
		}

		// Разбиваем стектрейс на строки для красивого логирования
		std::istringstream iss(stackTrace);
		std::string line;
		while (std::getline(iss, line))
		{
			ErrLog("%s", line.c_str());
		}
	}

	#ifdef _WIN32

	// Упрощенный callback для совместимости
	BOOL CALLBACK StackWalker::symCallback(HANDLE hProcess, ULONG ActionCode, ULONG64 CallbackData, ULONG64 UserContext)
	{

		// Базовая обработка - просто возвращаем TRUE
		switch (ActionCode)
		{
		case CBA_DEBUG_INFO:
			// Можно залогировать отладочную информацию
			break;
		default:
			break;
		}

		return TRUE;
	}

	// Альтернативная версия callback для старых SDK
	BOOL WINAPI StackWalker::symCallbackCompat(HANDLE hProcess, ULONG ActionCode, PVOID CallbackData, PVOID UserContext)
	{

		return symCallback(hProcess, ActionCode, (ULONG64)CallbackData, (ULONG64)UserContext);
	}

	std::string StackWalker::resolveAddress(DWORD64 address)
	{
		char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {};
		PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)symbolBuffer;
		symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbolInfo->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;

		if (SymFromAddr(m_process, address, &displacement, symbolInfo))
		{
			return symbolInfo->Name;
		}

		// Если не получилось получить символ, возвращаем сырой адрес
		return "UnknownFunction_" + toHexString(address);
	}

	std::string StackWalker::getModuleName(DWORD64 address)
	{
		IMAGEHLP_MODULE64 moduleInfo = {};
		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		if (SymGetModuleInfo64(m_process, address, &moduleInfo))
		{
			return moduleInfo.ModuleName;
		}

		return "UnknownModule";
	}

	std::string StackWalker::toHexString(DWORD64 value)
	{
		std::ostringstream oss;
		oss << std::hex << std::uppercase << value;
		return oss.str();
	}

	#endif
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
