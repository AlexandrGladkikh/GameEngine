#include "Logger.h"

#include <iostream>

namespace engine {

void Logger::logImpl(Level level, const std::string& message)
{
    switch (level) {
        case Level::INFO:
            std::cout << "[INFO] " << message << std::endl;
            break;
        case Level::WARNING:
            std::cout << "[WARNING] " << message << std::endl;
            break;
        case Level::ERROR:
            std::cout << "[ERROR] " << message << std::endl;
    }
}

}