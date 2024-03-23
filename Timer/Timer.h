#pragma once

#include <chrono>

#include "SharedTypes.h"

class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
public:
    using Time  = std::chrono::duration<double, std::milli>;

    bool start();
    Time stop();
};