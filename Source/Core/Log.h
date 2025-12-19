///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// Log realization
///////////////////////////////////////////////////////////////                       
//                \`*-.                   
//                 )  _`-.                
//                .  : `. .               
//                : _   '  \              
//                ; *` _.   `*-._         
//                `-.-'          `-.      
//                  ;       `       `.    
//                  :.       .        \   
//                  . \  .   :   .-'   .  
//                  '  `+.;  ;  '      :  
//                  :  '  |    ;       ;-.
//                  ; '   : :`-:     _.`* ;
//         [bug] .*' /  .*' ; .*`- +'  `*'
//               `*-*   `*-*  `*-*'       
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "CoreAPI.h"
#include <fstream>
#include "filesystem.h"
#include "windows.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	enum class LogLevel
	{
		debug,
		info,
		warning,
		error,
		unknown
	};

	class CORE_API CLog : public Patterns::Singleton<CLog>
	{
		friend class Patterns::Singleton<CLog>;

	  public:
		void Initialize(const std::string& logDir = LOGS);
		void Flush();
		void Destroy();

		void CreateConsole();
		void __cdecl Print(LPCSTR format, ...);
		void __cdecl PrintWithLevel(LPCSTR format, LogLevel level = LogLevel::error, ...);
		void __cdecl Debug_Print(LPCSTR format, ...);
		void __cdecl Warning_Print(LPCSTR format, ...);
		void __cdecl Error_Print(LPCSTR format, ...);

		bool IsInitialized() const
		{
			return m_Initialized;
		}
		const std::string& GetLogPath() const
		{
			return m_LogFilePath;
		}

	  private:
		CLog();
		~CLog();

		std::ofstream m_LogFileStream;
		std::string m_LogFilePath;
		bool m_Initialized = false;
		bool m_ConsoleCreated = false;

		// Цвета для консоли Windows
		enum ConsoleColor
		{
			BLACK = 0,
			DARK_BLUE = 1,
			DARK_GREEN = 2,
			DARK_CYAN = 3,
			DARK_RED = 4,
			DARK_MAGENTA = 5,
			DARK_YELLOW = 6,
			GRAY = 7,
			DARK_GRAY = 8,
			BLUE = 9,
			GREEN = 10,
			CYAN = 11,
			RED = 12,
			MAGENTA = 13,
			YELLOW = 14,
			WHITE = 15
		};

		void PrintInternal(LPCSTR format, va_list args, LogLevel level, bool isDebug = false);
		void SetConsoleColor(ConsoleColor color);
		void ResetConsoleColor();
		std::string GetLevelString(LogLevel level);
		ConsoleColor GetLevelColor(LogLevel level);
	};
} // namespace Core
///////////////////////////////////////////////////////////////
#define Log CLog::GetInstance().Print
#define Log2 CLog::GetInstance().PrintWithLevel
#define DbgLog CLog::GetInstance().Debug_Print
#define WarnLog CLog::GetInstance().Warning_Print
#define ErrLog CLog::GetInstance().Error_Print
#define LogInit(directory) CLog::GetInstance().Initialize(directory)
#define LogFlush() CLog::GetInstance().Flush()
#define LogDestroy() CLog::GetInstance().Destroy()
///////////////////////////////////////////////////////////////