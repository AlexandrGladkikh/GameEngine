#pragma once

#include <string>
#include <format>

namespace engine {

class Logger final {
public:
    template<typename... Args>
    static void info(const std::string& fmt_str, Args&&... args)
    {
        std::string message = std::vformat(fmt_str, std::make_format_args(args...));
        logImpl(Level::INFO, message);
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

private:
    enum class Level {
        INFO = 0,
        WARNING = 1,
        ERROR = 2
    };

    static void logImpl(Level level, const std::string& message);
};

}
