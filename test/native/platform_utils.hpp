/**
 * @file        platform_utils.hpp
 * @brief       Platform specific utilities.
 */
#ifndef PLATFORM_UTILS_HPP
#define PLATFORM_UTILS_HPP

#include <chrono>
#include <thread>

namespace utils
{

static inline void delay(std::chrono::milliseconds duration)
{
    std::this_thread::sleep_for(duration);
}

}; // namespace utils

#endif /* PLATFORM_UTILS_HPP */
