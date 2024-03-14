/**
 * @file        event_group.hpp
 * @brief       Event group on top of FreeRTOS. A wrapper for FreeRTOS Event Groups.
 */
#ifndef EVENT_GROUP_HPP
#define EVENT_GROUP_HPP

#include "jungles_os_helpers/utils/enum_to_bits.hpp"

#include <chrono>
#include <memory>
#include <optional>

// Unfortunately, we need to include FreeRTOSConfig.h here to statically assert on maximum number of event bits.
// TODO: Find a way to avoid this, because it makes the PIMPL pattern ineffective.
#include "FreeRTOSConfig.h"

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
    Bits wait_one(Bits, std::chrono::milliseconds);
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

static inline constexpr unsigned max_event_bits()
{
#ifdef configUSE_16_BIT_TICKS
    if constexpr (configUSE_16_BIT_TICKS == 1)
        return 8;
#endif

#ifdef configTICK_TYPE_WIDTH_IN_BITS
    static_assert(configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS
                      or configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS
                      or configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_64_BITS,
                  "Unsupported configTICK_TYPE_WIDTH_IN_BITS value");

    if constexpr (configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS)
        return 8;
    else if constexpr (configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS)
        return 24;
    else if constexpr (configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_64_BITS)
        // TODO: Support max 56 events bits for 64-bit tick type (requires different UnderlyingType).
        return 32;
    else // Never reached
        return 0;
#endif

    return 24;
}

}; // namespace detail

//! Many-to-one event group - might have multiple producers, but only one consumer.
template<auto... Events>
struct event_group
{
  private:
    using EnumToBits = utils::EnumToBits<Bits, Events...>;
    using EnumType = typename EnumToBits::value_type;

  public:
    static_assert(sizeof...(Events) <= detail::max_event_bits(), "Too many events for the underlying event group");

    template<auto... Evts>
    void set()
    {
        constexpr auto bits{enum_to_bits.template to_bits<Evts...>()};
        pimpl.set(bits);
    }

    Bits get()
    {
        return pimpl.get();
    }

    template<auto... Evts>
    EnumType wait_one()
    {
        constexpr auto bits{enum_to_bits.template to_bits<Evts...>()};
        auto event_bit{pimpl.wait_one(bits)};
        pimpl.clear(event_bit);
        return static_cast<EnumType>(detail::bit_position(event_bit));
    }

    template<auto... Evts>
    std::optional<EnumType> wait_one(std::chrono::milliseconds timeout)
    {
        constexpr auto bits{enum_to_bits.template to_bits<Evts...>()};
        auto event_bit{pimpl.wait_one(bits, timeout)};
        if (event_bit != 0)
        {
            pimpl.clear(event_bit);
            return static_cast<EnumType>(detail::bit_position(event_bit));
        } else
        {
            return std::nullopt;
        }
    }

    template<auto... Evts>
    void clear()
    {
        constexpr auto bits{enum_to_bits.template to_bits<Evts...>()};
        pimpl.clear(bits);
    }

  private:
    utils::EnumToBits<Bits, Events...> enum_to_bits;
    impl::event_group pimpl;
};

}; // namespace jungles::freertos

#endif /* EVENT_GROUP_HPP */
