/**
 * @file	message_pump.hpp
 * @brief	Interface for a message pump
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef INTERFACE_MESSAGE_PUMP_HPP
#define INTERFACE_MESSAGE_PUMP_HPP

namespace jungles
{

namespace interface
{

template<typename Message>
struct message_pump
{
    virtual void send(Message&&) = 0;
    virtual Message receive() = 0;

    virtual ~message_pump() = default;
};

} // namespace interface

} // namespace jungles

#endif /* INTERFACE_MESSAGE_PUMP_HPP */
