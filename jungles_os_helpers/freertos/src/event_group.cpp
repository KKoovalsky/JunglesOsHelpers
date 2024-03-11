/**
 * @file        event_group.cpp
 * @brief       Implements the Pimpl stuff for event_group to make the FreeRTOS  dependency private.
 */

#include "jungles_os_helpers/freertos/event_group.hpp"

#include "FreeRTOS.h"
#include "event_groups.h"
#include <memory>

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

  private:
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

} // namespace impl

} // namespace jungles::freertos
