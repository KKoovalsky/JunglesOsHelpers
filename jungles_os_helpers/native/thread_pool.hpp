/**
 * @file        thread_pool.hpp
 * @brief       The thread pool for the native target.
 */
#ifndef NATIVE_THREAD_POOL_HPP
#define NATIVE_THREAD_POOL_HPP

#include <mutex>

#include "jungles_os_helpers/generic/thread_pool.hpp"

#include "jungles_os_helpers/native/message_pump.hpp"
#include "jungles_os_helpers/native/thread.hpp"

namespace os::native
{

template<os::generic::RunnersCount RC>
using thread_pool = os::generic::thread_pool<RC, jungles::native::thread, jungles::native::message_pump, std::mutex>;

}

#endif /* NATIVE_THREAD_POOL_HPP */
