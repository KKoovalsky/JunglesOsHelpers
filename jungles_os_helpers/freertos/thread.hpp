/**
 * @file	thread.hpp
 * @brief	Implements a thread class run on top of FreeRTOS
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_THREAD_IMPL_HPP
#define FREERTOS_THREAD_IMPL_HPP

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <string_view>

namespace jungles
{

namespace freertos
{

namespace detail
{

struct thread_proxy
{
    std::function<void(void)> thread_fun;
    std::string name;
    StaticSemaphore_t task_finished_sem_storage = {};
    SemaphoreHandle_t task_finished_sem = {};
    std::shared_ptr<thread_proxy> ptr_to_myself;

    explicit thread_proxy(std::function<void(void)> tf, std::string&& name) : thread_fun{tf}, name{std::move(name)}
    {
        auto sem{xSemaphoreCreateBinaryStatic(&task_finished_sem_storage)};
        assert(sem != nullptr);
        task_finished_sem = sem;
    }

    void notify_task_finished()
    {
        xSemaphoreGive(task_finished_sem);
    }

    void wait_task_finished()
    {
        xSemaphoreTake(task_finished_sem, portMAX_DELAY);
    }

    ~thread_proxy()
    {
        if (task_finished_sem != nullptr)
            vSemaphoreDelete(task_finished_sem);
    }
};

} // namespace detail

class thread
{
  public:
    using thread_fun = std::function<void(void)>;

    thread(thread_fun tf, std::string name, unsigned stack_size, int priority) :
        m_data{std::make_shared<detail::thread_proxy>(tf, std::move(name))}
    {
        // We could put all the initialization into the proxy class but we are not able to because:
        // 1. We need to have m_data.get() pointer a valid pointer to an object which is fully instatiated when
        //    creating a Task.
        // 2. We could use std::enable_shared_from_this inside proxy class to assign self to ptr_to_myself, but
        //    one can't call std::shared_from_this() inside constructor of self.
        // This is why the proxy object is initialized partially and its initialization is not performed within
        // the object itself.

        // Make the cyclic reference to the proxy object. This will ensure that the proxy will still live when the
        // "master" (thread) object is destructed after detaching.
        m_data->ptr_to_myself = m_data;

        auto result{xTaskCreate(task_fun,
                                m_data->name.c_str(),
                                stack_size,
                                m_data.get(), // Forward the proxy object to the task code.
                                priority,
                                nullptr)};

        assert(result == pdPASS);
    }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    thread(thread&&) = delete;
    thread& operator=(thread&&) = delete;

    void join()
    {
        if (m_is_detached)
            throw already_detached_error{};
        else if (m_is_joined)
            return;

        m_data->wait_task_finished();

        m_is_joined = true;
    }

    void detach()
    {
        throw_if_joined_or_detached_already();

        m_is_detached = true;
    }

    ~thread()
    {
        if (m_is_detached)
            return;
        join();
    }

    struct error : public std::exception
    {
    };

    struct already_detached_error : public error
    {
    };

    struct already_joined_error : public error
    {
    };

  private:
    static inline void task_fun(void* raw_pointer_to_proxy);

    void throw_if_joined_or_detached_already()
    {
        if (m_is_detached)
            throw already_detached_error{};
        else if (m_is_joined)
            throw already_joined_error{};
    }

    std::shared_ptr<detail::thread_proxy> m_data;
    bool m_is_detached{false};
    bool m_is_joined{false};
};

inline void thread::task_fun(void* raw_pointer_to_proxy)
{
    // We need to put this block into separate section because we want to have all the shared_ptr's destructed before
    // the call to vTaskDelete(nullptr). The call to vTaskDelete may prevent this function to reach the final '}' (to
    // actually return) which may lead to incorrect, or no- destruction of the objects,
    {
        // Increase the reference count to not to destroy the object when the ptr_to_myself is reset.
        auto self{static_cast<detail::thread_proxy*>(raw_pointer_to_proxy)->ptr_to_myself};

        // Get rid of the cyclic reference.
        self->ptr_to_myself = nullptr;

        // Eventually, call the thread code (the user/client code) to be executed.
        self->thread_fun();

        self->notify_task_finished();
    }

    vTaskDelete(nullptr);
}

} // namespace freertos
} // namespace jungles

#endif /* FREERTOS_THREAD_IMPL_HPP */
