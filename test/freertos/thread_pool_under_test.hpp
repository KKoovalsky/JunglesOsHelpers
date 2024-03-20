/**
 * @file	thread_pool_under_test.hpp
 * @brief	Defines the Thread Pool to be tested for FreeRTOS target.
 */
#ifndef THREAD_POOL_UNDER_TEST_DEFINITION_HPP
#define THREAD_POOL_UNDER_TEST_DEFINITION_HPP

#include "jungles_os_helpers/freertos/thread_pool.hpp"

namespace test
{

template<os::generic::RunnersCount RC>
inline auto make_thread_pool()
{
    constexpr unsigned queue_size{16};
    constexpr freertos::ThreadPoolConfig config{.runner_stack_size = 2048, .runner_priority = 1};
    return freertos::thread_pool<RC, queue_size, config>();
}

} // namespace test

#endif /* THREAD_POOL_UNDER_TEST_DEFINITION_HPP */
