/**
 * @file	flag.hpp
 * @brief	Implements a thread-safe flag which is settable, resettable and waitable.
 * @author	Kacper Kowalski - kacper.kowalski@hum-systems.com
 */
#ifndef FREERTOS_FLAG_HPP
#define FREERTOS_FLAG_HPP

#include "flag.hpp"

#include "FreeRTOS.h"
#include "event_groups.h"

namespace jungles
{

namespace freertos
{

class flag
{
  public:
    flag() : event_group_handle{xEventGroupCreate()}
    {
    }

    ~flag()
    {
        vEventGroupDelete(event_group_handle);
    }

    void set()
    {
        xEventGroupSetBits(event_group_handle, event_bit);
    }

    void reset()
    {
        xEventGroupClearBits(event_group_handle, event_bit);
    }

    void wait()
    {
        auto do_not_clear_on_exit{pdFALSE};
        // Doesn't matter since we are waiting for a single bit.
        auto wait_for_all_bits{pdTRUE};
        auto wait_undefinitely{portMAX_DELAY};

        xEventGroupWaitBits(event_group_handle, event_bit, do_not_clear_on_exit, wait_for_all_bits, wait_undefinitely);
    }

    bool is_set() const
    {
        auto current_event_bits{xEventGroupGetBits(event_group_handle)};
        return current_event_bits == event_bit;
    }

  private:
    EventGroupHandle_t event_group_handle;
    static constexpr EventBits_t event_bit{0x01};
};

} // namespace freertos
} // namespace jungles

#endif /* FREERTOS_FLAG_HPP */
