/**
 * @file        platform_utils.hpp
 * @brief       Platform specific utilities.
 */
#ifndef PLATFORM_UTILS_HPP
#define PLATFORM_UTILS_HPP

#include <chrono>

#include "FreeRTOS.h"
#include "task.h"

namespace utils
{

static inline void delay(std::chrono::milliseconds duration)
{
    vTaskDelay(pdMS_TO_TICKS(duration.count()));
}

}; // namespace utils

#endif /* PLATFORM_UTILS_HPP */
