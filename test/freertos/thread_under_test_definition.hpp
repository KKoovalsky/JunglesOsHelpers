/**
 * @file	thread_under_test_definition.cpp
 * @brief	Defines the customization layer for the generic tests of thread.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "jungles_os_helpers/freertos/thread.hpp"

#include <memory>
#include <string>
#include <vector>

inline jungles::freertos::thread get_thread_for_test_run()
{
    static std::vector<std::string> thread_names;
    static unsigned thread_num{0};
    thread_names.push_back("Test_thread_" + std::to_string(thread_num));
    ++thread_num;
    return jungles::freertos::thread(thread_names.back(), 512, 1);
}
