/**
 * @file	flag_impl.hpp
 * @brief	Implements a thread-safe flaig which is settable, resettable and waitable.
 * @author	Kacper Kowalski - kacper.kowalski@hum-systems.com
 */
#ifndef FLAG_IMPL_HPP
#define FLAG_IMPL_HPP

#include "flag.hpp"

#include "FreeRTOS.h"
#include "event_groups.h"

namespace jungles
{

class flag_impl : public flag
{
  public:
    flag_impl() : event_group_handle{xEventGroupCreate()}
    {
    }

    ~flag_impl()
    {
        vEventGroupDelete(event_group_handle);
    }

    virtual void set() override
    {
        xEventGroupSetBits(event_group_handle, event_bit);
    }

    virtual void reset() override
    {
        xEventGroupClearBits(event_group_handle, event_bit);
    }

    virtual void wait() override
    {
        auto do_not_clear_on_exit{pdFALSE};
        // Doesn't matter since we are waiting for a single bit.
        auto wait_for_all_bits{pdTRUE};
        auto wait_undefinitely{portMAX_DELAY};

        xEventGroupWaitBits(event_group_handle, event_bit, do_not_clear_on_exit, wait_for_all_bits, wait_undefinitely);
    }

    virtual bool is_set() const override
    {
        auto current_event_bits{xEventGroupGetBits(event_group_handle)};
        return current_event_bits == event_bit;
    }

  private:
    EventGroupHandle_t event_group_handle;
    static constexpr EventBits_t event_bit{0x01};
};

} // namespace jungles

#endif /* FLAG_IMPL_HPP */
