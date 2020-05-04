/**
 * @file	os_delayed_job.hpp
 * @brief	Implements the interface of the os_delayed_job_interface for FreeRTOS.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef OS_DELAYED_JOB_HPP
#define OS_DELAYED_JOB_HPP

#include "../os_delayed_job_iface.hpp"

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"

#include <functional>
#include <memory>

namespace jungles
{

namespace detail
{

/**
 * \brief A proxy class used to implement detachable delayed job.
 *
 * This can only be created with std::shared_ptr. This object encapsulates all the data used with the os_delayed_job
 * implementation for FreeRTOS.
 * The most important member is m_ptr_to_myself, which is a shared pointer to this object. It's used to outlive
 * the call to os_delayed_job::start() function. When the callback is called then this pointer is reset to be able
 * to destruct the object. This approach has been chosen to be able to detach the job, so a proxy object of this type
 * will not be destructed when the os_delayed_job is destructed.
 */
struct os_delayed_job_data
{
    explicit os_delayed_job_data(std::function<void(void)> timeout_callback) : m_timeout_callback{timeout_callback} {}

    ~os_delayed_job_data()
    {
        // The object of this proxy class will be handled by shared_ptr so it will be always destructed last.
        // That means in turn that this is the safest place to deallocate all the resources.

        if (m_timer != nullptr) { xTimerDelete(m_timer, portMAX_DELAY); }
        if (m_job_finished_sem != nullptr) { vSemaphoreDelete(m_job_finished_sem); }
    }

    StaticSemaphore_t m_job_finished_sem_storage = {};
    std::function<void(void)> m_timeout_callback;
    std::shared_ptr<os_delayed_job_data> m_ptr_to_myself;
    TimerHandle_t m_timer = {};
    SemaphoreHandle_t m_job_finished_sem = {};
};

} // namespace detail

class os_delayed_job : public os_delayed_job_interface
{
  public:
    //! Is move constructible only.
    os_delayed_job() = default;
    os_delayed_job(const os_delayed_job&) = delete;
    os_delayed_job& operator=(const os_delayed_job&) = delete;
    os_delayed_job(os_delayed_job&&) = default;
    os_delayed_job& operator=(os_delayed_job&&) = default;

    virtual os_error start(unsigned delay_ms, std::function<void(void)> callback) override
    {
        m_data = std::make_shared<detail::os_delayed_job_data>(callback);

        auto sem{xSemaphoreCreateBinaryStatic(&m_data->m_job_finished_sem_storage)};
        if (sem == nullptr) return os_error::creation_failed;
        m_data->m_job_finished_sem = sem;

        auto no_auto_reload{pdFALSE};
        auto tim{xTimerCreate("os_delayed_job",
                              pdMS_TO_TICKS(delay_ms),
                              no_auto_reload,
                              m_data.get(), // Pass the proxy object to the timer callback
                              timer_callback)};
        if (tim == nullptr) return os_error::creation_failed;
        m_data->m_timer = tim;

        // This is very important. There is a shared ownership to itself, so it means that as long as the
        // m_ptr_to_myself is not reset explicitly, then we could make a memory leak.
        m_data->m_ptr_to_myself = m_data;

        if (xTimerStart(tim, pdMS_TO_TICKS(1)) == pdFAIL)
        {
            m_data->m_ptr_to_myself = nullptr;
            return os_error::start_failed;
        }

        return os_error::ok;
    }

    virtual os_error detach() override
    {
        if (!m_data) return os_error::error;

        m_is_detached = true;
        return os_error::ok;
    }

    ~os_delayed_job()
    {
        if (m_is_detached) return;
        if (!m_data) return;

        // If not detached then wait for the callback to be called
        xSemaphoreTake(m_data->m_job_finished_sem, portMAX_DELAY);
    }

  private:
    static inline void timer_callback(TimerHandle_t);

    std::shared_ptr<detail::os_delayed_job_data> m_data;
    bool m_is_detached{false};
};

void os_delayed_job::timer_callback(TimerHandle_t tim)
{
    // The proxy class is passed as a void*, because this is how C allows you to do it only.
    // We capture the timer ID which was passed as the fourth argument to the xTimerCreateStatic function in the
    // start() function.
    auto data_ptr{static_cast<detail::os_delayed_job_data*>(pvTimerGetTimerID(tim))};

    // Will effectively create a shared_ptr, so we will increase the reference count to keep the object alive when
    // resetting m_ptr_to_myself in the next line.
    auto data{data_ptr->m_ptr_to_myself};
    data->m_ptr_to_myself = nullptr;

    // Do the delayed job.
    data->m_timeout_callback();

    // Inform os_delayed_job that we have finished and it is free to destroy itself. Will be ignored when detached.
    xSemaphoreGive(data->m_job_finished_sem);
}

} // namespace jungles

#endif /* OS_DELAYED_JOB_HPP */
