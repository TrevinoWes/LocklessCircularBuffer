#include "Timer.h"

bool Timer::start() {
    start_ = std::chrono::high_resolution_clock::now();
    return true;
};

Timer::Time Timer::stop() {
    auto end = std::chrono::high_resolution_clock::now();
    Time length = end - start_;
    return length;
};
