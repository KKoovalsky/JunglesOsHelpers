/**
 * @file        event_group.cpp
 * @brief       Implements the Pimpl stuff for event_group to make the FreeRTOS  dependency private.
 */

#include "jungles_os_helpers/freertos/event_group.hpp"

#include <memory>

#include "FreeRTOS.h"
#include "event_groups.h"
#include "portmacro.h"

namespace jungles::freertos
{

namespace impl
{

struct event_group_impl
{
    event_group_impl() : handle{xEventGroupCreate()}
    {
    }

    ~event_group_impl()
    {
        vEventGroupDelete(handle);
    }

    void set(Bits bits)
    {
        xEventGroupSetBits(handle, bits);
    }

    Bits get()
    {
        return xEventGroupGetBits(handle);
    }

    Bits wait_one(Bits bits)
    {
        return do_wait_one(bits);
    }

    void clear(Bits bits)
    {
        xEventGroupClearBits(handle, bits);
    }

  private:
    static inline Bits get_first_set_bit(Bits bits)
    {
        Bits mask{1};
        while ((bits & mask) == 0)
            mask <<= 1;
        return mask;
    }

    Bits do_wait_one(Bits bits, TickType_t delay = portMAX_DELAY)
    {
        auto do_not_clear_on_exit{pdFALSE};
        auto do_not_wait_for_all{pdFALSE};
        auto bits_set{xEventGroupWaitBits(handle, bits, do_not_clear_on_exit, do_not_wait_for_all, delay)};
        return get_first_set_bit(bits_set);
    }

    EventGroupHandle_t handle;
};

event_group::event_group() : pimpl{std::make_unique<event_group_impl>()}
{
}

event_group::~event_group()
{
}

void event_group::set(Bits bits)
{
    pimpl->set(bits);
}

Bits event_group::get()
{
    return pimpl->get();
}

Bits event_group::wait_one(Bits bits)
{
    return pimpl->wait_one(bits);
}

void event_group::clear(Bits bits)
{
    return pimpl->clear(bits);
}

} // namespace impl

} // namespace jungles::freertos
