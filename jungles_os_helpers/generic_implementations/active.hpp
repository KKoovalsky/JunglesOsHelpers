/**
 * @file	active.hpp
 * @brief	Generic implementation of an Active object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef GENERIC_IMPLEMENTATIONS_ACTIVE_HPP
#define GENERIC_IMPLEMENTATIONS_ACTIVE_HPP

#include <functional>
#include <variant>

namespace jungles
{

namespace generic
{

/**
 * Active object implementation from "Prefer Using Active Objects Instead of Naked Threads" by Herb Sutter
 * \tparam Message Type of message to be passed around.
 * \tparam JoinableThread Thread implementing join() and start(functor) functions.
 * \tparam MessagePumpTemplate Template for a message pump hanlding MessageProxy
 */
template<typename Message, template<typename> typename MessagePumpTemplate, typename JoinableThread>
class active
{
  public:
    using MessageHandler = std::function<void(Message&&)>;

    //! Used to inform the active that it shall exit and stop the thread loop
    struct quit_message
    {
    };
    using MessageProxy = typename std::variant<Message, quit_message>;

    using MessagePump = MessagePumpTemplate<MessageProxy>;

    active(MessageHandler message_handler, MessagePump& message_pump_impl, JoinableThread& thread_impl) :
        message_handler{std::move(message_handler)}, message_pump_impl{message_pump_impl}, thread_impl{thread_impl}
    {
        thread_impl.start([this]() { this->thread_code(); });
    }

    void send(Message&& m)
    {
        message_pump_impl.send({std::move(m)});
    }

    ~active()
    {
        message_pump_impl.send(quit_message{});
        thread_impl.join();
    }

  private:
    void thread_code()
    {
        while (true)
        {
            auto msg{message_pump_impl.receive()};
            if (std::holds_alternative<quit_message>(msg))
                break;

            auto user_msg{std::get<Message>(msg)};
            this->message_handler(std::move(user_msg));
        }
    }

    MessageHandler message_handler;
    MessagePump& message_pump_impl;
    JoinableThread& thread_impl;
};

} // namespace generic
} // namespace jungles

#endif /* GENERIC_IMPLEMENTATIONS_ACTIVE_HPP */

