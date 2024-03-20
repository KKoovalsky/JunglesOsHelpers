/**
 * @file	mutex_under_test.hpp
 * @brief	Defines the Thread Pool to be tested for FreeRTOS target.
 */
#ifndef MUTEX_UNDER_TEST_DEFINITION_HPP
#define MUTEX_UNDER_TEST_DEFINITION_HPP

#include <mutex>

namespace test
{

inline std::mutex make_mutex()
{
    return {};
}

} // namespace test

#endif /* MUTEX_UNDER_TEST_DEFINITION_HPP */
