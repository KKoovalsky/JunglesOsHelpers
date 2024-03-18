/**
 * @file	thread_pool_under_test_definition.hpp
 * @brief	Defines the Thread Pool to be tested for native target.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef THREAD_POOL_UNDER_TEST_DEFINITION_HPP
#define THREAD_POOL_UNDER_TEST_DEFINITION_HPP

#include "jungles_os_helpers/native/thread_pool.hpp"

namespace test
{

template<os::generic::RunnersCount RC>
auto make_thread_pool()
{
    return os::native::thread_pool<RC>();
}

} // namespace test

#endif /* THREAD_POOL_UNDER_TEST_DEFINITION_HPP */
