/**
 * @file	test_thread_impl.cpp
 * @brief	Defines the customization layer for the generic tests of thread.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "thread.hpp"

#include "freertos/thread_impl.hpp"
#include <memory>
#include <vector>
#include <string>


std::unique_ptr<jungles::thread> get_thread_for_test_run(std::function<void(void)> f)
{
    static std::vector<std::string> thread_names;
    static unsigned thread_num{0};
    thread_names.push_back("Test_thread_" + std::to_string(thread_num));
    ++thread_num;
    return std::make_unique<jungles::thread_impl<512>>(f, thread_names.back(), 1);
}
