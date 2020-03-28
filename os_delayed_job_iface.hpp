/**
 * @file	os_delayed_job_iface.hpp
 * @brief	Interface used to define a delayed job - a job executed after specific point in time.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef OS_DELAYED_JOB_IFACE_HPP
#define OS_DELAYED_JOB_IFACE_HPP

#include <functional>

#include "os_def.hpp"

namespace jungles
{

/**
 * \brief Delayed job interface, which performs a job after specified time.
 *
 * This is an interface, because:
 * - to have an interface for classes which might be used across various operating systems. So when using this
 *   interface and one needs to switch the firmware to a different platform then he can easily do it, unless there is
 *   an implementation for the targer operating system.
 * - to provide a simple interface for other classes which want to implement something on top of such an object. Then
 *   it can us that interface to separate the test layer and mock this kind of time dependent classes.
 */
class os_delayed_job_interface
{
  public:
    //! Register a delayed job and call it after delay_ms. job_callback is the function to be called after delay_ms.
    virtual os_error start(unsigned delay_ms, std::function<void(void)> job_callback) = 0;

    //! Detach the delayed job, so allow this object to be destructed, but don't cancel the job.
    virtual os_error detach() = 0;

    //! If detach() has not been called then block on destruction until the callback is called.
    virtual ~os_delayed_job_interface() {}
};

} // namespace jungles

#endif /* OS_DELAYED_JOB_IFACE_HPP */
