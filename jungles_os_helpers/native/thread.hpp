/**
 * @file	thread.hpp
 * @brief	Thread implementation compatible with this library.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef JUNGLES_NATIVE_THREAD_HPP
#define JUNGLES_NATIVE_THREAD_HPP

#include <thread>

namespace jungles
{

namespace native
{

class thread 
{
  public:
    void start(std::function<void(void)> f)
    {
        underlying_thread = std::thread{std::move(f)};
    }

    void join()
    {
        underlying_thread.join();
    }

  private:
    std::thread underlying_thread;
};

} // namespace native

} // namespace jungles

#endif /* THREAD_HPP */
