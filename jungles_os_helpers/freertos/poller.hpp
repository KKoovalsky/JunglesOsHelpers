/**
 * @file	freertos_poller.hpp
 * @brief	Defines aliases for a FreeRTOS based poller.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_POLLER_HPP
#define FREERTOS_POLLER_HPP

#include "jungles_os_helpers/generic_implementations/poller.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace jungles
{

namespace freertos
{

template<unsigned PollingIntervalInMilliseconds, unsigned TimeoutInMilliseconds>
static constexpr auto make_poller()
{
    return jungles::generic::poller<PollingIntervalInMilliseconds, TimeoutInMilliseconds>(
        [](auto delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); });
};

} // namespace freertos

}; // namespace jungles

#endif /* FREERTOS_POLLER_HPP */
