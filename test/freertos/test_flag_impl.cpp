/**
 * @file	test_flag_impl.cpp
 * @brief	Defines the implementation for flag for the generic test.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include <memory>

#include "freertos/flag_impl.hpp"

std::unique_ptr<jungles::flag> get_flag_implementation_under_test()
{
    return std::make_unique<jungles::flag_impl>();
}
