/**
 * @file	message_pump.hpp
 * @brief	Message pump implementation for a native build
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef NATIVE_MESSAGE_PUMP_HPP
#define NATIVE_MESSAGE_PUMP_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

namespace jungles
{

namespace native
{

template<typename Message>
class message_pump
{
  public:
    void send(Message&& m)
    {
        {
            std::lock_guard g{mux};
            queue.push(std::move(m));
        }
        cv.notify_all();
    }

    Message receive()
    {
        std::unique_lock ul{mux};
        cv.wait(ul, [this]() { return !queue.empty(); });
        auto r{std::move(queue.front())};
        queue.pop();
        return r;
    }

  private:
    std::condition_variable cv;
    std::mutex mux;
    std::queue<Message> queue;
};

} // namespace native

} // namespace jungles

#endif /* NATIVE_MESSAGE_PUMP_HPP */
