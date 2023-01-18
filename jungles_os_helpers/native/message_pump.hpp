/**
 * @file	message_pump.hpp
 * @brief	Message pump implementation for a native build
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef NATIVE_MESSAGE_PUMP_HPP
#define NATIVE_MESSAGE_PUMP_HPP

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace jungles::native
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
        return pop()
    }

    std::optional<Message> receive_immediate()
    {
        std::lock_guard g{mux};
        if (queue.empty())
            return std::nullopt;
        return pop();
    }

  private:
    //! Call only if it is certain that the queue is not empty, and the mux must be taken while calling this function.
    Message pop()
    {
        auto r{std::move(queue.front())};
        queue.pop();
        return r;
    }

    std::condition_variable cv;
    std::mutex mux;
    std::queue<Message> queue;
};

} // namespace jungles::native

#endif /* NATIVE_MESSAGE_PUMP_HPP */
