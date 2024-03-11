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

    void set(Bits);
    Bits get();
    Bits wait_one(Bits);
    void clear(Bits);

  private:
    std::unique_ptr<event_group_impl> pimpl;
};

}; // namespace impl

namespace detail
{

static inline constexpr unsigned bit_position(unsigned val)
{
    // Assume 'val' is always positive and power of two
    unsigned position{0};
    while ((val & (1 << position)) == 0)
        position++;

    return position;
}

}; // namespace detail

template<auto... Events>
struct event_group
{
  private:
    using EnumToBits = utils::EnumToBits<Bits, Events...>;
    using EnumType = typename EnumToBits::value_type;

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

    template<typename... E>
    EnumType wait_one(E... events)
    {
        auto bits{enum_to_bits.to_bits(events...)};
        auto event_bit{pimpl.wait_one(bits)};
        pimpl.clear(event_bit);
        return static_cast<EnumType>(detail::bit_position(event_bit));
    }

    template<typename... E>
    void clear(E... events)
    {
        auto bits{enum_to_bits.to_bits(events...)};
        pimpl.clear(bits);
    }

  private:
    utils::EnumToBits<Bits, Events...> enum_to_bits;
    impl::event_group pimpl;
};

}; // namespace jungles::freertos

#endif /* EVENT_GROUP_HPP */
