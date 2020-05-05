/**
 * @file	active_impl.hpp
 * @brief	Implements generic active object
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef ACTIVE_IMPL_GENERIC__HPP
#define ACTIVE_IMPL_GENERIC__HPP

#include "active.hpp"
#include "queue.hpp"
#include "thread.hpp"

#include <memory>
#include <variant>

namespace jungles
{

// TODO: C++20 - thread that has concept of specific interface, to not have typename Thread and extra unique_ptr?
template<typename Message, typename Thread, typename Queue>
class active_generic_impl : active<Message>
{
    active_generic_impl(typename active<Message>::on_message_received_handler message_handler) :
        active<Message>{message_handler},
        message_pump{std::make_unique<Queue<MessageProxy>>()}, // TODO: test if that works?
        worker_thread{std::make_unique<Thread>([this]() { this->thread_code(); })}

    {}

  private:
    void thread_code()
    {
        while (true)
        {
            auto wait_undefinitely_for_message{[this]() {
                // As we wait undefinitely then we are sure that the message will be there always, thus we can
                // call with * safely.
                return *message_pump.receive(-1);
            }};

            auto msg{wait_undefinitely_for_message()};
            if (std::holds_alternative<quit_message>(msg)) break;

            auto user_msg{std::get<Message>(msg)};
            this->m_message_handler(std::move(user_msg));
        }
    }

    //! Used to inform the active that it shall exit and stop the thread loop
    struct quit_message
    {};
    using MessageProxy = typename std::variant<Message, quit_message>;

    std::unique_ptr<jungles::queue<MessageProxy>> message_pump;
    std::unique_ptr<jungles::thread> worker_thread;
};

} // namespace jungles

#endif /* ACTIVE_IMPL_GENERIC__HPP */
