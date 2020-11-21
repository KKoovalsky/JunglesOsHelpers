/**
 * @file	test_active_impl.cpp
 * @brief	Defines the customization layer for the generic tests of active.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "jungles_os_helpers/freertos/active.hpp"

#include "../test_helpers.hpp"

#include <memory>

std::unique_ptr<jungles::active<test_helpers::message>>
get_active_object_for_test_run(typename jungles::active<test_helpers::message>::on_message_received_handler f)
{
    return std::make_unique<jungles::freertos::active_impl<test_helpers::message, 1024, 8>>(f);
}
