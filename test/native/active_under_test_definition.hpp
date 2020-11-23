/**
 * @file	active_under_test_definition.hpp
 * @brief	Defines the Active to be tested for native target.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef ACTIVE_UNDER_TEST_DEFINITION_HPP
#define ACTIVE_UNDER_TEST_DEFINITION_HPP

#include "jungles_os_helpers/native/active.hpp"

template<typename Message>
auto get_active_object_for_test_run(std::function<void(Message&&)> f)
{
    return jungles::native::active<Message>(f);
}

#endif /* ACTIVE_UNDER_TEST_DEFINITION_HPP */
