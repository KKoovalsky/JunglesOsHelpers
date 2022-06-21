/**
 * @file        poller2.hpp
 * @brief       A poller which polls the predicate with specific interval and timeout.
 * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef POLLER2_HPP
#define POLLER2_HPP

#include <concepts>
#include <functional>

namespace jungles
{

struct DelayMilliseconds
{
    constexpr DelayMilliseconds(unsigned v) : value{v}
    {
    }

    const unsigned value;
};

struct TimeoutMilliseconds
{
    constexpr TimeoutMilliseconds(unsigned v) : value{v}
    {
    }

    const unsigned value;
};

struct PollIntervalMilliseconds
{
    constexpr PollIntervalMilliseconds(unsigned v) : value{v}
    {
    }

    const unsigned value;
};

namespace generic
{

template<auto MillisecondDelayer>
requires std::regular_invocable<decltype(MillisecondDelayer), DelayMilliseconds>
static inline bool
poll(PollIntervalMilliseconds interval_ms, TimeoutMilliseconds timeout_ms, std::function<bool(void)> predicate)
{
    auto is_interval_multiply_of_timeout{(timeout_ms.value % interval_ms.value) == 0};

    unsigned num_requests_to_timeout{(timeout_ms.value / interval_ms.value)
                                     + (is_interval_multiply_of_timeout ? 0 : 1)};

    for (unsigned i{0}; i < num_requests_to_timeout; ++i)
    {
        if (predicate())
            return true;
        MillisecondDelayer(interval_ms.value);
    }

    return false;
}

} // namespace generic

} // namespace jungles

#endif /* POLLER2_HPP */
