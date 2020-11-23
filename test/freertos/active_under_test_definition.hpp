/**
 * @file	active_under_test_definition.hpp
 * @brief	Defines the Active to be tested for native target.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef ACTIVE_UNDER_TEST_DEFINITION_HPP
#define ACTIVE_UNDER_TEST_DEFINITION_HPP

#include "jungles_os_helpers/freertos/active.hpp"

template<typename Message>
auto get_active_object_for_test_run(std::function<void(Message&&)> f)
{
    return jungles::freertos::active<Message, 16>(f, "freertos_active", 1024, 1);
}

#endif /* ACTIVE_UNDER_TEST_DEFINITION_HPP */
