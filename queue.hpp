/**
 * @file	queue.hpp
 * @brief	Defines interface for a thread-safe queue.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cstddef>
#include <tl/expected.hpp>

#include "os_def.hpp"

namespace jungles
{

//! Thread-safe queue used to send messages.
template<typename ElementType, std::size_t Size>
struct queue
{
    /**
     * \brief Sends a message to the queue.
     *
     * \retval os_error::ok when successful
     * \retval os_error::queue_full if the queue is full and the timeout has passed
     * \returns os_error::error or other for the implementation specifc error.
     */
    virtual os_error send(ElementType&&) = 0;

    /**
     * \brief Receives a message with the specified timeout.
     *
     * \returns An instance of ElementType when could obtain a message from the queue within the specified time.
     * \retval os_error::queue_empty when couldn't obtain a message from the queue within the specified time.
     * \returns An enumeration of os_error != os_error::ok, if other error occured.
     */
    virtual tl::expected<ElementType, os_error> receive(unsigned timeout_ms) = 0;

    virtual ~queue() = default;
};

} // namespace jungles

#endif /* QUEUE_HPP */
