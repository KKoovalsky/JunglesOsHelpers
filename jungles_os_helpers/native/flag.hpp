/**
 * @file	flag.hpp
 * @brief	Atomic flag with blocking wait.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FLAG_HPP
#define FLAG_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace jungles
{

class flag
{
  public:
    void set()
    {
        std::lock_guard g{mux};
        flag = true;
        cv.notify_all();
    }

    void wait()
    {
        std::unique_lock lk{mux};
        cv.wait(lk, [this]() { return flag; });
    }

    template<class Rep, class Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& duration)
    {
        std::unique_lock lk{mux};
        return cv.wait_for(lk, duration, [this]() { return flag; });
    }

    bool is_set()
    {
        std::lock_guard g{mux};
        return flag;
    }

  private:
    std::mutex mux;
    std::condition_variable cv;
    bool flag{false};
};

} // namespace jungles

#endif /* FLAG_HPP */
