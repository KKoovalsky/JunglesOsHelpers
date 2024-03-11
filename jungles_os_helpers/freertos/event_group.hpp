/**
 * @file        event_group.hpp
 * @brief       Event group on top of FreeRTOS. A wrapper for FreeRTOS Event Groups.
 */
#ifndef EVENT_GROUP_HPP
#define EVENT_GROUP_HPP

#include "jungles_os_helpers/utils/enum_to_bits.hpp"

#include <memory>

namespace jungles::freertos
{

using Bits = unsigned;

namespace impl
{

struct event_group_impl;

struct event_group
{
    event_group();
    ~event_group();
    void set(Bits bits);
    Bits get();

  private:
    std::unique_ptr<event_group_impl> pimpl;
};

}; // namespace impl
template<auto... Events>
struct event_group
{
  public:
    template<typename... E>
    void set(E... events)
    {
        auto bits{enum_to_bits.to_bits(events...)};
        pimpl.set(bits);
    }

    Bits get()
    {
        return pimpl.get();
    }

  private:
    utils::EnumToBits<Bits, Events...> enum_to_bits;
    impl::event_group pimpl;
};

}; // namespace jungles::freertos

#endif /* EVENT_GROUP_HPP */
