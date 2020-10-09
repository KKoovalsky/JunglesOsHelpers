/**
 * @file poller.hpp
 * @author Kacper Kowalski (kacper.s.kowalski@gmail.com)
 * @brief Implements an poller which polls a predicate with a specific interval, for the specified time.
 * @date 2020-07-31
 */
#ifndef __POLLER_HPP__
#define __POLLER_HPP__

#include <functional>

namespace jungles
{

template<unsigned PollingIntervalInMilliseconds, unsigned TimeoutInMilliseconds>
class poller
{
  public:
    using MillisecondDelayer = std::function<void(unsigned millisecond_delay)>;

    explicit poller(MillisecondDelayer millisecond_delayer) : millisecond_delayer{millisecond_delayer}
    {
    }

    bool poll(std::function<bool(void)> predicate)
    {
        static constexpr auto is_interval_multiply_of_timeout{(TimeoutInMilliseconds % PollingIntervalInMilliseconds)
                                                              == 0};
        static constexpr unsigned num_requests_to_timeout{(TimeoutInMilliseconds / PollingIntervalInMilliseconds)
                                                          + (is_interval_multiply_of_timeout ? 0 : 1)};

        for (unsigned i{0}; i < num_requests_to_timeout; ++i)
        {
            if (predicate())
                return true;
            millisecond_delayer(PollingIntervalInMilliseconds);
        }
        return false;
    }

  private:
    MillisecondDelayer millisecond_delayer;
};

}; // namespace jungles

#endif // __POLLER_HPP__
