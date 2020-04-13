/**
 * @file	test_thread_impl.cpp
 * @brief	Defines the customization layer for the generic tests of thread.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "thread.hpp"

#include "freertos/thread_impl.hpp"
#include <memory>


std::unique_ptr<jungles::thread> get_thread_for_test_run(std::function<void(void)> f)
{
    return std::make_unique<jungles::thread_impl<512>>(f, "Test thread", 1);
}
