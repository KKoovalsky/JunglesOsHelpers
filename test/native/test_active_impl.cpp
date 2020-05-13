/**
 * @file	test_active_impl.cpp
 * @brief	Tests the generic implementation of the active object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "active_impl.hpp"
#include "os_def.hpp"

#include "../test_helpers.hpp"

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template<typename Message>
class message_pump_mock
{
  public:
    jungles::os_error send(Message&& m) 
    {
        {
            std::lock_guard g{mux};
            queue.push(std::move(m));
        }
        cv.notify_all();
        return jungles::os_error::ok;
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

std::unique_ptr<jungles::active<test_helpers::message>>
get_active_object_for_test_run(typename jungles::active<test_helpers::message>::on_message_received_handler f)
{
    return std::make_unique<jungles::active_generic_impl<test_helpers::message, std::thread, message_pump_mock>>(f);
}
