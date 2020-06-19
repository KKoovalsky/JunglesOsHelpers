/**
 * @file	flag.hpp
 * @brief	Atomic flag with blocking wait.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FLAG_HPP
#define FLAG_HPP

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

  private:
    std::mutex mux;
    std::condition_variable cv;
    bool flag;
};

} // namespace jungles

#endif /* FLAG_HPP */
