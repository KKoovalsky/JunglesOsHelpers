/**
 * @file	thread_impl.hpp
 * @brief	Implements a thread class run on top of FreeRTOS
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef THREAD_IMPL_HPP
#define THREAD_IMPL_HPP

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "thread.hpp"

#include <array>
#include <cassert>
#include <functional>
#include <memory>

namespace jungles
{

namespace detail
{

template<std::size_t StackSize>
struct thread_impl_proxy;

}; // namespace detail

//! Implements the thread interface. See jungles::thread documentation for details
template<std::size_t StackSize>
class thread_impl : public thread
{
  public:
    using thread_fun = std::function<void(void)>;

    thread_impl(thread_fun tf, const std::string& name, int priority) :
        m_data{std::make_shared<detail::thread_impl_proxy<StackSize>>(tf, name)}
    {
        // We could put all the initialization into the proxy class but we are not able to because:
        // 1. We need to have m_data.get() pointer a valid pointer to an object which is fully instatiated when
        //    creating a Task.
        // 2. We could use std::enable_shared_from_this inside proxy class to assign self to ptr_to_myself, but
        //    one can't call std::shared_from_this() inside constructor of self.
        // This is why the proxy object is initialized partially and its initialization is not performed within
        // the object itself.

        auto handle{xTaskCreateStatic(task_fun,
                                      m_data->name.c_str(),
                                      StackSize,
                                      m_data.get(), // Forward the proxy object to the task code.
                                      priority,
                                      m_data->stack_storage.data(),
                                      &m_data->task_storage)};

        assert(handle != nullptr);

        // Make the cyclic reference to the proxy object. This will ensure that the proxy will still live when the
        // "master" (thread_impl) object is destructed after detaching.
        m_data->ptr_to_myself = m_data;
    }

    thread_impl(const thread_impl&) = delete;
    thread_impl& operator=(const thread_impl&) = delete;
    thread_impl(thread_impl&&) = delete;
    thread_impl& operator=(thread_impl&&) = delete;

    virtual os_error join() override
    {
        if (m_is_detached)
            return os_error::error;
        else if (m_is_joined)
            return os_error::ok;

        m_data->wait_task_finished();

        m_is_joined = true;

        return os_error::ok;
    }

    virtual os_error detach() override
    {
        if (m_is_detached || m_is_joined) return os_error::error;

        m_is_detached = true;
        return os_error::ok;
    }

    ~thread_impl()
    {
        if (m_is_detached) return;
        join();
    }

  private:
    static inline void task_fun(void* raw_pointer_to_proxy);

    std::shared_ptr<detail::thread_impl_proxy<StackSize>> m_data;
    bool m_is_detached{false};
    bool m_is_joined{false};
};

namespace detail
{

template<std::size_t StackSize>
struct thread_impl_proxy
{
    std::array<StackType_t, StackSize> stack_storage = {};
    StaticTask_t task_storage = {};
    std::function<void(void)> thread_fun;
    std::string name;
    StaticSemaphore_t task_finished_sem_storage = {};
    SemaphoreHandle_t task_finished_sem = {};
    std::shared_ptr<thread_impl_proxy<StackSize>> ptr_to_myself;

    explicit thread_impl_proxy(std::function<void(void)> tf, const std::string& name) : thread_fun{tf}, name{name}
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

    ~thread_impl_proxy()
    {
        if (task_finished_sem != nullptr) vSemaphoreDelete(task_finished_sem);
    }
};

}; // namespace detail

template<std::size_t StackSize>
inline void thread_impl<StackSize>::task_fun(void* raw_pointer_to_proxy)
{
    // We need to put this block into separate section because we want to have all the shared_ptr's destructed before
    // the call to vTaskDelete(nullptr). The call to vTaskDelete may prevent this function to reach the final '}' (to
    // actually return) which may lead to incorrect, or no- destruction of the objects,
    {
        // Increase the reference count to not to destroy the object when the ptr_to_myself is reset.
        auto self{static_cast<detail::thread_impl_proxy<StackSize>*>(raw_pointer_to_proxy)->ptr_to_myself};

        // Get rid of the cyclic reference.
        self->ptr_to_myself = nullptr;

        // Eventually, call the thread code (the user/client code) to be executed.
        self->thread_fun();

        self->notify_task_finished();
    }

    vTaskDelete(nullptr);
}

} // namespace jungles

#endif /* THREAD_IMPL_HPP */
