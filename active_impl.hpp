/**
 * @file	active_impl.hpp
 * @brief	Implements generic active object
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef ACTIVE_IMPL_GENERIC__HPP
#define ACTIVE_IMPL_GENERIC__HPP

#include "active.hpp"

#include <memory>
#include <variant>

namespace jungles
{

/**
 * Active object implementation from "Prefer Using Active Objects Instead of Naked Threads" by Herb Sutter
 * \tparam Message Type of message to be passed around.
 * \tparam Thread Class that takes void(void) function in constructor and implements join() function (like std::thread)
 * \tparam MessagePump shall implement jungles::queue interface.
 * \todo This documentation shall be inside a C++20 concept.
 */
template<typename Message, typename Thread, template<typename> typename MessagePump>
class active_generic_impl : public active<Message>
{
  public:
    active_generic_impl(const active_generic_impl&) = delete;
    active_generic_impl& operator=(const active_generic_impl&) = delete;
    active_generic_impl(active_generic_impl&&) = delete;
    active_generic_impl& operator=(active_generic_impl&&) = delete;

    active_generic_impl(typename active<Message>::on_message_received_handler message_handler) :
        active<Message>{message_handler}, worker_thread{[this]() { this->thread_code(); }}
    {}

    virtual os_error send(Message&& m) override
    {
        return message_pump.send({std::move(m)});
    }

    virtual ~active_generic_impl()
    {
        message_pump.send(quit_message{});
        worker_thread.join();
    }

  private:
    //! Used to inform the active that it shall exit and stop the thread loop
    struct quit_message
    {};
    using MessageProxy = typename std::variant<Message, quit_message>;

    MessagePump<MessageProxy> message_pump;
    Thread worker_thread;

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
};

} // namespace jungles

#endif /* ACTIVE_IMPL_GENERIC__HPP */
