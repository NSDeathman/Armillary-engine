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
			return true;

#if defined(_WIN32) && defined(_DEBUG)
		m_process = GetCurrentProcess();

		// 1. Инициализируем символы
		SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);

		// 2. Получаем путь к папке с exe/dll, чтобы найти там .pdb
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::filesystem::path exePath = path;
		std::string searchPath = exePath.parent_path().string(); // Папка bin/

		// 3. Инициализируем с явным путем поиска
		if (!SymInitialize(m_process, searchPath.c_str(), TRUE))
		{
			// Если не вышло, пробуем стандартно
			SymInitialize(m_process, nullptr, TRUE);
		}

		m_initialized = true;
		Log("StackWalker initialized (Search path: %s)", searchPath.c_str());
#endif

		return m_initialized;
	}

	void StackWalker::shutdown()
	{
		if (!m_initialized)
		{
			return;
		}

	#if defined(_WIN32) && defined(_DEBUG)
		SymCleanup(m_process);
	#endif

		m_initialized = false;
		Log("StackWalker shutdown");
	}

	std::vector<StackFrame> StackWalker::captureStackTrace(int maxFrames, int skipFrames, PCONTEXT contextRecord)
	{
		std::vector<StackFrame> frames;

		// Защита: если инициализация не прошла, нет смысла продолжать
		if (!m_initialized || !m_process)
		{
			StackFrame frame;
			frame.functionName = "StackWalker not initialized";
			frames.push_back(frame);
			return frames;
		}

	// Используем макрос _WIN64 (стандарт MSVC), WIN64 может быть не определен
#if defined(_WIN32) && defined(_DEBUG) && (defined(_WIN64) || defined(WIN64))

		HANDLE thread = GetCurrentThread();
		CONTEXT context = {};

		// 1. Подготовка контекста
		if (contextRecord)
		{
			context = *contextRecord;
		}
		else
		{
			RtlCaptureContext(&context);
		}

		// 2. Настройка структуры кадра
		STACKFRAME64 stackFrame = {};
		DWORD machineType = IMAGE_FILE_MACHINE_AMD64;

		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
		stackFrame.AddrStack.Mode = AddrModeFlat;

		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrFrame.Offset = context.Rbp;
		stackFrame.AddrStack.Offset = context.Rsp;

		// === КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ ДЛЯ ACCESS VIOLATION ===
		// Если программа попыталась выполнить код по адресу 0 (RIP=0),
		// StackWalk64 сразу же вернет FALSE и завершит работу.
		// Мы обязаны добавить этот "нулевой" кадр вручную, чтобы увидеть ошибку.
		if (contextRecord)
		{
			StackFrame crashFrame;
			DWORD64 crashAddr = context.Rip;

			if (crashAddr == 0)
			{
				// Ошибка вызова по нулевому указателю
				crashFrame.address = "0x00000000";
				crashFrame.functionName = "[CRASH] EXECUTION AT NULL POINTER";
				crashFrame.moduleName = "Unknown";

				// Если у нас есть PDB, DbgHelp иногда может сказать, откуда пришли в nullptr
				// через регистр возврата на стеке (RSP), но пока запишем факт падения.
			}
			else
			{
				crashFrame.address = toHexString(crashAddr);
				// resolveAddress безопасна и не упадет на плохом адресе
				crashFrame.functionName = resolveAddress(crashAddr) + " [CRASH SITE]";
				crashFrame.moduleName = getModuleName(crashAddr);

				DWORD displacement = 0;
				IMAGEHLP_LINE64 lineInfo = {};
				lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

				if (SymGetLineFromAddr64(m_process, crashAddr, &displacement, &lineInfo))
				{
					crashFrame.fileName = lineInfo.FileName;

					// Упрощаем путь к файлу
					size_t lastSlash = crashFrame.fileName.find_last_of("\\/");
					if (lastSlash != std::string::npos)
						crashFrame.fileName = crashFrame.fileName.substr(lastSlash + 1);

					crashFrame.lineNumber = std::to_string(lineInfo.LineNumber);
				}
			}
			frames.push_back(crashFrame);
		}
		// ========================================================

		int frameCount = 0;

		// Если мы только что упали по адресу 0, StackWalk не сможет начать оттуда.
		// Нам нужно "обмануть" его или начать с адреса возврата, который лежит на стеке (RSP).
		// Простой вариант - если PC == 0, попробовать прочитать верхушку стека.
		if (stackFrame.AddrPC.Offset == 0 && stackFrame.AddrStack.Offset != 0)
		{
			DWORD64 returnAddr = 0;
			// Читаем 8 байт (адрес возврата) по адресу указателя стека
			if (ReadProcessMemory(m_process, (LPCVOID)stackFrame.AddrStack.Offset, &returnAddr, sizeof(returnAddr),
								  NULL))
			{
				stackFrame.AddrPC.Offset = returnAddr; // "Воскрешаем" указатель инструкции
				// Увеличиваем стек (так как адрес возврата был "снят")
				stackFrame.AddrStack.Offset += 8;
			}
		}

		// 3. Цикл обхода стека
		int skipped = 0;

		while (frameCount < maxFrames)
		{
			// Основная магия Windows API
			BOOL result = StackWalk64(machineType, m_process, thread, &stackFrame, &context, nullptr,
									  SymFunctionTableAccess64, SymGetModuleBase64, nullptr);

			if (!result || stackFrame.AddrPC.Offset == 0)
			{
				break;
			}

			// Если мы вручную добавили первый кадр выше, StackWalk может вернуть его же
			// первым в цикле. Нужно отфильтровать дубликат.
			if (!frames.empty())
			{
				std::string currentAddrHex = toHexString(stackFrame.AddrPC.Offset);
				if (frames.back().address == currentAddrHex ||
					(frames.back().address == "0x00000000" && stackFrame.AddrPC.Offset == 0))
				{
					continue;
				}
			}

			// Логика пропуска (skipFrames)
			if (skipped < skipFrames)
			{
				skipped++;
				continue;
			}

			StackFrame frame;
			frame.address = toHexString(stackFrame.AddrPC.Offset);
			frame.functionName = resolveAddress(stackFrame.AddrPC.Offset);
			frame.moduleName = getModuleName(stackFrame.AddrPC.Offset);

			DWORD displacement = 0;
			IMAGEHLP_LINE64 lineInfo = {};
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (SymGetLineFromAddr64(m_process, stackFrame.AddrPC.Offset, &displacement, &lineInfo))
			{
				frame.fileName = lineInfo.FileName;
				size_t lastSlash = frame.fileName.find_last_of("\\/");
				if (lastSlash != std::string::npos)
				{
					frame.fileName = frame.fileName.substr(lastSlash + 1);
				}
				frame.lineNumber = std::to_string(lineInfo.LineNumber);
			}

			frames.push_back(frame);
			frameCount++;
		}

#else
		StackFrame frame;
		frame.functionName = "Stack walking disabled (Release/x86 or non-Windows)";
		frames.push_back(frame);
#endif

		// Последняя проверка: если вектор все еще пуст, значит все упало
		if (frames.empty())
		{
			StackFrame err;
			err.functionName = "<Stack Trace Empty - Failed to unwind>";
			frames.push_back(err);
		}

		return frames;
	}

	std::string StackWalker::getStackTraceString(int maxFrames, int skipFrames, PCONTEXT contextRecord)
	{
		// Если контекст передан, skipFrames обычно нужен 0, так как мы уже в нужном месте
		auto frames = captureStackTrace(maxFrames, skipFrames, contextRecord);

		std::ostringstream oss;
		oss << "Stack trace (" << frames.size() << " frames):" << std::endl;
		for (size_t i = 0; i < frames.size(); ++i)
		{
			oss << "#" << i << " " << frames[i].toString() << std::endl;
		}
		return oss.str();
	}

	void StackWalker::logStackTrace(const std::string& context, PCONTEXT contextRecord)
	{
		int skip = contextRecord ? 0 : 2;
		auto stackTrace = getInstance().getStackTraceString(32, skip, contextRecord);

		if (!context.empty())
			ErrLog("Stack trace for: %s", context.c_str());

		std::istringstream iss(stackTrace);
		std::string line;
		while (std::getline(iss, line))
		{
			ErrLog("%s", line.c_str());
		}
	}

	void StackWalker::loadModule(const std::string& img, const std::string& mod, DWORD64 baseAddr, DWORD size)
	{
#if defined(_WIN32) && defined(_DEBUG)
		if (!m_initialized)
			return;

		DWORD64 result = SymLoadModule64(m_process, NULL, img.c_str(), mod.c_str(), baseAddr, size);

		if (result == 0)
		{
			// Можно добавить GetLastError() для отладки самого отладчика
			ErrLog("Failed to load symbols for %s. Error: %d", mod.c_str(), GetLastError());
		}
		else
		{
			Log("Loaded symbols for %s", mod.c_str());
		}
#endif
	}

	#if defined(_WIN32) && defined(_DEBUG)

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
