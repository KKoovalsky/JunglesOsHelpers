/**
 * @file	mutex_under_test.hpp
 * @brief	Defines the Thread Pool to be tested for FreeRTOS target.
 */
#ifndef MUTEX_UNDER_TEST_DEFINITION_HPP
#define MUTEX_UNDER_TEST_DEFINITION_HPP

#include "jungles_os_helpers/freertos/mutex.hpp"

namespace test
{

inline jungles::freertos::mutex make_mutex()
{
    return {};
}

} // namespace test

#endif /* MUTEX_UNDER_TEST_DEFINITION_HPP */
