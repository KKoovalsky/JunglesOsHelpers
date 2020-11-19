/**
 * @file	queue.hpp
 * @brief	Defines interface for a thread-safe queue.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <optional>

namespace jungles
{

//! Thread-safe queue used to send messages.
template<typename ElementType>
struct queue
{
    //! Sends a message to the queue.
    virtual void send(ElementType&&) = 0;

    //! Receives a message from the queue with the specified timeout. Returns no element on timeout.
    virtual std::optional<ElementType> receive(unsigned timeout_ms) = 0;

    virtual ~queue() = default;
};

} // namespace jungles

#endif /* QUEUE_HPP */
