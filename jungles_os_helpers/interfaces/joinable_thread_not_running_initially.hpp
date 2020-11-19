/**
 * @file	joinable_thread_not_running_initially.hpp
 * @brief	Interface for a thread that is not running initially.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef JOINABLE_THREAD_NOT_RUNNING_INITALLY_HPP
#define JOINABLE_THREAD_NOT_RUNNING_INITALLY_HPP

#include <functional>

namespace jungles
{

namespace interface
{

struct joinable_thread_not_running_initially
{
    virtual void start(std::function<void(void)>) = 0;
    virtual void join() = 0;

    virtual ~joinable_thread_not_running_initially() = default;
};

} // namespace interface

} // namespace jungles

#endif /* JOINABLE_THREAD_NOT_RUNNING_INITALLY_HPP */
