#include "Utils.h"

#include <random>
#include <chrono>

namespace engine {

uint32_t generateUniqueId()
{
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen();
}

}