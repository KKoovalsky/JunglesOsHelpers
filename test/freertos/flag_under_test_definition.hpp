/**
 * @file	test_flag_impl.cpp
 * @brief	Defines the implementation for flag for the generic test.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "jungles_os_helpers/freertos/flag.hpp"

namespace test
{

inline jungles::freertos::flag make_flag()
{
    return jungles::freertos::flag{};
}

} // namespace test
