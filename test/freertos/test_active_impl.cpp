/**
 * @file	test_active_impl.cpp
 * @brief	Defines the customization layer for the generic tests of active.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "active.hpp"
#include "freertos/active_impl.hpp"

#include "../test_helpers.hpp"

#include <memory>

std::unique_ptr<jungles::active<test_helpers::message>>
get_active_object_for_test_run(typename jungles::active<test_helpers::message>::on_message_received_handler f)
{
    return std::make_unique<jungles::active_impl<test_helpers::message, 8, 1024>>(f, 1);
}
