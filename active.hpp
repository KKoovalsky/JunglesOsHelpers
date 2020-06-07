/**
 * @file	active.hpp
 * @brief	Defines an interface for an Active Object (like in the article "Prefer Using Active Objects Instead of
 *              Naked Threads" - Dr. Dobb's; June 14, 2010; H. Sutter
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef ACTIVE_HPP
#define ACTIVE_HPP

#include <functional>

namespace jungles
{

/**
 * Shall start a background worker (e.g. a thread) which is ready to receive and handle messages through send().
 */
template<typename Message>
class active
{
  public:
    using on_message_received_handler = std::function<void(Message&&)>;

    explicit active(on_message_received_handler message_handler) : m_message_handler{message_handler} {}

    //! Send a message to the background worker, so commission a job for it.
    virtual void send(Message&&) = 0;

    //! Shall stop and join the background worker.
    virtual ~active() = default;

  protected:
    //! Sub-class shall initialize this member.
    on_message_received_handler m_message_handler;
};

} // namespace jungles

#endif /* ACTIVE_HPP */
