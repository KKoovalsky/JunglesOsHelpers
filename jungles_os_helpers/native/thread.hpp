/**
 * @file	thread.hpp
 * @brief	Thread implementation compatible with this library.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef JUNGLES_NATIVE_THREAD_HPP
#define JUNGLES_NATIVE_THREAD_HPP

#include "jungles_os_helpers/interfaces/joinable_thread_not_running_initially.hpp"

#include <thread>

namespace jungles
{

namespace native
{

class thread : public jungles::interface::joinable_thread_not_running_initially
{
  public:
    virtual void start(std::function<void(void)> f) override
    {
        underlying_thread = std::thread{std::move(f)};
    }

    virtual void join() override
    {
        underlying_thread.join();
    }

  private:
    std::thread underlying_thread;
};

} // namespace native

} // namespace jungles

#endif /* THREAD_HPP */
