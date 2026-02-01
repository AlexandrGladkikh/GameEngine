#include "Logger.h"

#include <iostream>
#include <format>
#include <chrono>

namespace engine {

std::string timestamp()
{
    using namespace std::chrono;
    const auto now = floor<seconds>(system_clock::now());
    const zoned_time local_time{current_zone(), now};
    return std::format("{:%Y-%m-%d %H:%M:%S}", local_time);
}

Level Logger::m_level = Level::DEBUG;

void Logger::setLogLevel(Level level)
{
    m_level = level;
}

void Logger::logImpl(Level level, const std::string& message)
{
    if (level < Logger::m_level) {
        return;
    }

    switch (level) {
        case Level::INFO:
            std::cout << timestamp() << " [INFO] " << message << std::endl;
            break;
        case Level::DEBUG:
            std::cout << timestamp() << " [DEBUG] " << message << std::endl;
            break;
        case Level::WARNING:
            std::cout << timestamp() << " [WARNING] " << message << std::endl;
            break;
        case Level::ERROR:
            std::cout << timestamp() << " [ERROR] " << message << std::endl;
    }
}

}