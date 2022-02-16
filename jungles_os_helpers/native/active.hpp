/**
 * @file	active.hpp
 * @brief	Active for a native target able to use std::thread, std::condition_variable, ...
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef JUNGLES_NATIVE_ACTIVE_HPP
#define JUNGLES_NATIVE_ACTIVE_HPP

#include "jungles_os_helpers/generic/active.hpp"
#include "jungles_os_helpers/native/message_pump.hpp"
#include "jungles_os_helpers/native/thread.hpp"

namespace jungles
{

namespace native
{

template<typename Message>
class active
{
  private:
    using Thread = jungles::native::thread;

    template<typename T>
    using MessagePumpTemplate = jungles::native::message_pump<T>;

    using Active = jungles::generic::active<Message, MessagePumpTemplate, Thread>;
    using MessagePump = typename Active::MessagePump;

  public:
    active(typename Active::MessageHandler message_handler) :
        underlying_active{message_handler, message_pump_impl, thread_impl}
    {
    }

    void send(Message&& m)
    {
        underlying_active.send(std::move(m));
    }

  private:
    MessagePump message_pump_impl;
    Thread thread_impl;
    Active underlying_active;
};

} // namespace native

} // namespace jungles

#endif /* JUNGLES_NATIVE_ACTIVE_HPP */
