/**
 * @file	thread.hpp
 * @brief	Interface for a thread object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef THREAD_HPP
#define THREAD_HPP

namespace jungles
{

//! An interface for a thread. It is assumed that at the moment of creation the thread is started.
struct thread
{
    //! Blocks until the thread finishes.
    virtual void join() = 0;

    //! Permits the thread to execute after the instance being an owner of the thread is destructed.
    virtual void detach() = 0;

    //! Shall join if not detached.
    virtual ~thread() = default;
};

}; // namespace jungles

#endif /* THREAD_HPP */

