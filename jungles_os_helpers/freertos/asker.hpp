/**
 * @file	freertos_asker.hpp
 * @brief	Defines aliases for a FreeRTOS based asker.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_ASKER_HPP
#define FREERTOS_ASKER_HPP

#include "asker.hpp"

#include "FreeRTOS.h"

namespace jungles
{

namespace freertos
{

template<unsigned AskingIntervalInMilliseconds, unsigned TimeoutInMilliseconds>
static constexpr auto make_asker()
{
    return asker<AskingIntervalInMilliseconds, TimeoutInMilliseconds>(
        [](auto delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); });
};

}

}; // namespace jungles

#endif /* FREERTOS_ASKER_HPP */
