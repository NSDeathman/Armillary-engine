#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace Armillary 
{

    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger
    {
    public:
        static Logger& GetInstance();

        void SetLogLevel(LogLevel level);
        void EnableConsoleOutput(bool enable);
        void EnableFileOutput(const std::string& filepath);

        void Log(LogLevel level, const std::string& message);

        void Debug(const std::string& msg) { Log(LogLevel::Debug, msg); }
        void Info(const std::string& msg) { Log(LogLevel::Info, msg); }
        void Warning(const std::string& msg) { Log(LogLevel::Warning, msg); }
        void Error(const std::string& msg) { Log(LogLevel::Error, msg); }

    private:
        Logger();
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::string GetTimestamp() const;
        std::string LevelToString(LogLevel level) const;
        void WriteToOutput(const std::string& formatted);

        LogLevel m_CurrentLevel = LogLevel::Info;
        bool m_ConsoleOutput = true;
        std::ofstream m_FileStream;
    };

} // namespace Armillary

#define LOG_DEBUG(msg)    Armillary::Logger::GetInstance().Debug(msg)
#define LOG_INFO(msg)     Armillary::Logger::GetInstance().Info(msg)
#define LOG_WARNING(msg)  Armillary::Logger::GetInstance().Warning(msg)
#define LOG_ERROR(msg)    Armillary::Logger::GetInstance().Error(msg)

#define LOG(msg) Armillary::Logger::GetInstance().Info(msg)