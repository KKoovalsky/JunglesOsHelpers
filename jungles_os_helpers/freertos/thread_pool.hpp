/**
 * @file        thread_pool.hpp
 * @brief       The thread pool for the FreeRTOS target.
 */
#ifndef FREERTOS_THREAD_POOL_HPP
#define FREERTOS_THREAD_POOL_HPP

#include <mutex>

#include "jungles_os_helpers/generic/thread_pool.hpp"

#include "jungles_os_helpers/freertos/mutex.hpp"
#include "jungles_os_helpers/freertos/queue.hpp"
#include "jungles_os_helpers/freertos/thread.hpp"

namespace freertos
{

struct ThreadPoolConfig
{
    const std::size_t runner_stack_size;
    const std::size_t runner_priority;
};

namespace detail
{

template<os::generic::RunnersCount RC, std::size_t QueueSize, ThreadPoolConfig Config>
struct thread_pool_impl
{
    template<typename ElementType>
    using Queue = jungles::freertos::queue<ElementType, QueueSize>;

    using Mutex = jungles::freertos::mutex;

    struct Thread : jungles::freertos::thread
    {
        Thread() : jungles::freertos::thread{"ThreadFromPool", Config.runner_stack_size, Config.runner_priority}
        {
        }
    };
    using type = os::generic::thread_pool<RC, Thread, Queue, Mutex>;
};

}; // namespace detail

template<os::generic::RunnersCount RC, std::size_t QueueSize, ThreadPoolConfig TPC>
using thread_pool = typename detail::thread_pool_impl<RC, QueueSize, TPC>::type;

} // namespace freertos

#endif /* FREERTOS_THREAD_POOL_HPP */
