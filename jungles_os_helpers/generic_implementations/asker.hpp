/**
 * @file asker.hpp
 * @author Kacper Kowalski (kacper.s.kowalski@gmail.com)
 * @brief Implements an asker which asks a predicate with a specific interval, for the specified time.
 * @date 2020-07-31
 */
#ifndef __ASKER_HPP__
#define __ASKER_HPP__

#include <functional>

namespace jungles
{

namespace generic
{

template<unsigned AskingIntervalInMilliseconds, unsigned TimeoutInMilliseconds>
class asker
{
  public:
    using MillisecondDelayer = std::function<void(unsigned millisecond_delay)>;

    explicit asker(MillisecondDelayer millisecond_delayer) : millisecond_delayer{millisecond_delayer}
    {
    }

    bool ask(std::function<bool(void)> predicate)
    {
        static constexpr auto is_interval_multiply_of_timeout{(TimeoutInMilliseconds % AskingIntervalInMilliseconds)
                                                              == 0};
        static constexpr unsigned num_requests_to_timeout{(TimeoutInMilliseconds / AskingIntervalInMilliseconds)
                                                          + (is_interval_multiply_of_timeout ? 0 : 1)};

        for (unsigned i{0}; i < num_requests_to_timeout; ++i)
        {
            if (predicate())
                return true;
            millisecond_delayer(AskingIntervalInMilliseconds);
        }
        return false;
    }

  private:
    MillisecondDelayer millisecond_delayer;
};

} // namespace generic
} // namespace jungles

#endif // __ASKER_HPP__
