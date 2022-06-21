/**
 * @file	poller2.hpp
 * @brief	Defines alias for a FreeRTOS based poller.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_POLLER2_HPP
#define FREERTOS_POLLER2_HPP

#include "jungles_os_helpers/generic/poller2.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace jungles
{

namespace freertos
{

static inline constexpr void delay(DelayMilliseconds delay_ms)
{
    vTaskDelay(pdMS_TO_TICKS(delay_ms.value));
}

static inline constexpr auto poll{generic::poll<delay>};

} // namespace freertos

}; // namespace jungles

#endif /* FREERTOS_POLLER2_HPP */
