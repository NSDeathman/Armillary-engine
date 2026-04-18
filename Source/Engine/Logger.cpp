#include "pch.h"
#include "Logger.h"

namespace Armillary {

    Logger& Logger::GetInstance()
    {
        static Logger instance;
        return instance;
    }

    Logger::Logger() = default;

    Logger::~Logger()
    {
        if (m_FileStream.is_open())
            m_FileStream.close();
    }

    void Logger::SetLogLevel(LogLevel level)
    {
        m_CurrentLevel = level;
    }

    void Logger::EnableConsoleOutput(bool enable)
    {
        m_ConsoleOutput = enable;
    }

    void Logger::EnableFileOutput(const std::string& filepath)
    {
        if (m_FileStream.is_open())
            m_FileStream.close();
        m_FileStream.open(filepath, std::ios::out | std::ios::app);
        if (!m_FileStream.is_open())
        {
            std::cerr << "Failed to open log file: " << filepath << std::endl;
        }
    }

    std::string Logger::GetTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time_t);
#else
        tm_buf = *std::localtime(&time_t);
#endif

        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    std::string Logger::LevelToString(LogLevel level) const
    {
        switch (level)
        {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN";
        }
    }

    void Logger::WriteToOutput(const std::string& formatted)
    {
        if (m_ConsoleOutput)
        {
            std::cout << formatted << std::endl;
        }

        if (m_FileStream.is_open())
        {
            m_FileStream << formatted << std::endl;
            m_FileStream.flush(); // чтобы сразу записывалось на диск
        }

        // Для Visual Studio можно также дублировать в OutputDebugString
#ifdef _WIN32
        OutputDebugStringA((formatted + "\n").c_str());
#endif
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        if (level < m_CurrentLevel)
            return;

        std::stringstream formatted;
        formatted << "[" << GetTimestamp() << "] "
            << "[" << LevelToString(level) << "] "
            << message;

        WriteToOutput(formatted.str());
    }

} // namespace Armillary
