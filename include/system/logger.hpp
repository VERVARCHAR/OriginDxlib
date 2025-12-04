#pragma once
#include <fstream>
#include <string>

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
    static void Init(const std::string &filename = "logInfo.txt")
    {
        instance().ofs.open(filename, std::ios::out | std::ios::trunc);
    }

    static void Log(const std::string &msg, LogLevel level = LogLevel::Debug)
    {
        instance().ofs << "[" << LevelToStr(level) << "] " << msg << std::endl;
    }

    static void Close()
    {
        instance().ofs.close();
    }

private:
    std::ofstream ofs;

    static Logger &instance()
    {
        static Logger inst;
        return inst;
    }

    static const char *LevelToStr(LogLevel lv)
    {
        switch (lv)
        {
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        }
        return "";
    }
};
