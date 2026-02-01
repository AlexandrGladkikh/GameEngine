#pragma once

#include <string>
#include <format>

namespace engine {
    
enum class Level {
    INFO = 0,
    DEBUG = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger final {
public:
    template<typename... Args>
    static void info(const std::string& fmt_str, Args&&... args)
    {
        std::string message = std::vformat(fmt_str, std::make_format_args(args...));
        logImpl(Level::INFO, message);
    }
    
    template<typename... Args>
    static void debug(const std::string& fmt_str, Args&&... args)
    {
        std::string message = std::vformat(fmt_str, std::make_format_args(args...));
        logImpl(Level::DEBUG, message);
    }

    template<typename... Args>
    static void warning(const std::string& fmt_str, Args&&... args)
    {
        std::string message = std::vformat(fmt_str, std::make_format_args(args...));
        logImpl(Level::WARNING, message);
    }

    template<typename... Args>
    static void error(const std::string& fmt_str, Args&&... args)
    {
        std::string message = std::vformat(fmt_str, std::make_format_args(args...));
        logImpl(Level::ERROR, message);
    }

    static void setLogLevel(Level level);

private:
    static Level m_level;

    static void logImpl(Level level, const std::string& message);
};

}
