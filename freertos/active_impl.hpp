/**
 * @file	active_impl.hpp
 * @brief	Implements the active class run on top of FreeRTOS
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "FreeRTOS.h"

#include "active.hpp"
#include "freertos/queue_impl.hpp"
#include "freertos/thread_impl.hpp"

#include <cstddef>
#include <variant>

namespace jungles
{

template<typename Message, std::size_t MessagePumpSize, std::size_t UnderlyingThreadStackSize>
class active_impl : public active<Message>
{
  public:
    active_impl(typename active<Message>::on_message_received_handler message_handler, int priority) :
        active<Message>{message_handler}, worker_thread{[this]() { this->thread_code(); }, "active", priority}
    {}

    ~active_impl()
    {
        message_pump.send(quit_message{});
    }

    virtual os_error send(Message&& m) override
    {
        return message_pump.send({std::move(m)});
    }

  private:
    //! Used to inform the active that it shall exit and stop the thread loop
    struct quit_message
    {};
    using MessageProxy = typename std::variant<Message, quit_message>;

    void thread_code()
    {
        while (true)
        {
            auto wait_undefinitely_for_message{[this]() {
                // As we wait undefinitely then we are sure that the message will be there alwayas, thus we ca
                // call with * safely.
                return *message_pump.receive(-1);
            }};

            auto msg{wait_undefinitely_for_message()};
            if (std::holds_alternative<quit_message>(msg)) break;

            auto user_msg{std::get<Message>(msg)};
            this->m_message_handler(std::move(user_msg));
        }
    }

    queue_impl<MessageProxy, MessagePumpSize> message_pump;
    thread_impl<UnderlyingThreadStackSize> worker_thread;
};

}; // namespace jungles
