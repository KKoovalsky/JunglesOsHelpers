/**
 * @file	active.hpp
 * @brief	Active implementation running under FreeRTOS.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_ACTIVE_HPP
#define FREERTOS_ACTIVE_HPP

#include "jungles_os_helpers/freertos/queue.hpp"
#include "jungles_os_helpers/freertos/thread.hpp"
#include "jungles_os_helpers/generic/active.hpp"

namespace jungles
{

namespace freertos
{

template<typename Message, std::size_t MessagePumpSize>
class active
{
  public:
    using Thread = jungles::freertos::thread;

    template<typename T>
    using MessagePumpTemplate = jungles::freertos::queue<T, MessagePumpSize>;

    using Active = jungles::generic::active<Message, MessagePumpTemplate, Thread>;
    using MessagePump = typename Active::MessagePump;
    using MessageHandler = typename Active::MessageHandler;

  public:
    active(MessageHandler message_handler,
           std::string underlying_thread_name,
           unsigned underlying_thread_stack_size,
           int underlying_thread_priority) :
        underlying_thread{std::move(underlying_thread_name), underlying_thread_stack_size, underlying_thread_priority},
        the_active{message_handler, underlying_message_pump, underlying_thread}
    {
    }

    void send(Message&& m)
    {
        the_active.send(std::move(m));
    }

  private:
    MessagePump underlying_message_pump;
    Thread underlying_thread;
    Active the_active;
};

} // namespace freertos

} // namespace jungles

#endif /* FREERTOS_ACTIVE_HPP */
