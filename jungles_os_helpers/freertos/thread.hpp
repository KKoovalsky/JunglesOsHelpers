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
#include <string>
#include <string_view>

namespace jungles
{

namespace freertos
{

class thread
{
  public:
    using ThreadCode = std::function<void(void)>;

    thread(std::string name, unsigned stack_size, int priority) :
        thread_owner_shared_pointer{std::make_shared<thread_owner>(std::move(name), stack_size, priority)}
    {
    }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    thread(thread&&) = delete;
    thread& operator=(thread&&) = delete;

    void start(ThreadCode thread_code)
    {
        is_started = true;
        thread_owner_shared_pointer->start(std::move(thread_code));
    }

    void join()
    {
        if (is_detached)
            throw already_detached_error{};
        else if (is_joined)
            return;
        else if (!is_started)
            throw not_started_error{};

        thread_owner_shared_pointer->wait_task_finished();

        is_joined = true;
    }

    void detach()
    {
        if (is_detached)
            throw already_detached_error{};
        else if (is_joined)
            throw already_joined_error{};
        else if (!is_started)
            throw not_started_error{};

        is_detached = true;
    }

    ~thread()
    {
        if (is_detached or !is_started)
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

    struct not_started_error : public error
    {
    };

  private:
    struct thread_owner : public std::enable_shared_from_this<thread_owner>
    {
        explicit thread_owner(std::string&& name, unsigned stack_size, int priority) :
            task_finished_sem{xSemaphoreCreateBinaryStatic(&task_finished_sem_storage)},
            name{std::move(name)},
            stack_size{stack_size},
            priority{priority}
        {
            assert(task_finished_sem != nullptr);
        }

        void start(ThreadCode&& thread_code)
        {
            this->thread_code = std::move(thread_code);
            shared_ptr_to_myself = shared_from_this();
            auto result{xTaskCreate(task_fun,
                                    name.c_str(),
                                    stack_size,
                                    shared_ptr_to_myself.get(), // Forward self to the task code.
                                    priority,
                                    nullptr)};
            assert(result == pdPASS);
            (void) result;
        }

        void notify_task_finished()
        {
            xSemaphoreGive(task_finished_sem);
        }

        void wait_task_finished()
        {
            xSemaphoreTake(task_finished_sem, portMAX_DELAY);
        }

        ~thread_owner()
        {
            vSemaphoreDelete(task_finished_sem);
        }

        static inline void task_fun(void* raw_pointer_to_thread_owner)
        {
            auto with_shared_pointer_to_thread_owner_being_self_destructed_before_call_to_vTaskDelete{
                [=](std::function<void(std::shared_ptr<thread_owner> &&)> handler) {
                    auto self{static_cast<thread_owner*>(raw_pointer_to_thread_owner)->shared_ptr_to_myself};
                    handler(std::move(self));
                }};

            with_shared_pointer_to_thread_owner_being_self_destructed_before_call_to_vTaskDelete([](auto&& self) {
                auto remove_cyclic_reference_to_self{[](auto& self) {
                    self->shared_ptr_to_myself = nullptr;
                }};

                auto run_users_thread_code{[](auto& self) {
                    self->thread_code();
                }};

                remove_cyclic_reference_to_self(self);
                run_users_thread_code(self);
                self->notify_task_finished();
            });

            vTaskDelete(nullptr);
        }

        StaticSemaphore_t task_finished_sem_storage = {};
        SemaphoreHandle_t task_finished_sem = {};
        std::shared_ptr<thread_owner> shared_ptr_to_myself;
        std::string name;
        unsigned stack_size;
        int priority;
        ThreadCode thread_code;
    };

    std::shared_ptr<thread_owner> thread_owner_shared_pointer;
    bool is_detached{false};
    bool is_joined{false};
    bool is_started{false};
};

} // namespace freertos
} // namespace jungles

#endif /* FREERTOS_THREAD_IMPL_HPP */
