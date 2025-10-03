///////////////////////////////////////////////////////////////
// Created: 01.10.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"

#include <string>
#include <vector>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif
///////////////////////////////////////////////////////////////
namespace Core::Debug
{
	struct CORE_API StackFrame
	{
		std::string address;
		std::string functionName;
		std::string fileName;
		std::string lineNumber;
		std::string moduleName;

		std::string toString() const
		{
			std::ostringstream oss;
			oss << "[" << address << "] " << functionName;
			if (!fileName.empty())
			{
				oss << " in " << fileName;
				if (!lineNumber.empty())
				{
					oss << ":" << lineNumber;
				}
			}
			if (!moduleName.empty())
			{
				oss << " (" << moduleName << ")";
			}
			return oss.str();
		}
	};

	class CORE_API StackWalker
	{
	  public:
		static StackWalker& getInstance();

		bool initialize();
		void shutdown();

		std::vector<StackFrame> captureStackTrace(int maxFrames = 62, int skipFrames = 0);
		std::string getStackTraceString(int maxFrames = 62, int skipFrames = 0);

		static void logStackTrace(const std::string& context = "");

	  private:
		StackWalker() = default;
		~StackWalker() = default;

		bool m_initialized = false;
		HANDLE m_process = nullptr;

	#ifdef _WIN32
		static BOOL CALLBACK symCallback(HANDLE hProcess, ULONG ActionCode, ULONG64 CallbackData, ULONG64 UserContext);

		static BOOL WINAPI symCallbackCompat(HANDLE hProcess, ULONG ActionCode, PVOID CallbackData, PVOID UserContext);

		std::string resolveAddress(DWORD64 address);
		std::string getModuleName(DWORD64 address);
		std::string toHexString(DWORD64 value);
	#endif
	};
} // namespace Core::Debug
///////////////////////////////////////////////////////////////
