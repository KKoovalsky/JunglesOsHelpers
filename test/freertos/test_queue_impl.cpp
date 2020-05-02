/**
 * @file	test_queue_impl.cpp
 * @brief	Defines the instance to be injected to the queue generic tests
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "freertos/queue_impl.hpp"

#include <memory>

std::unique_ptr<jungles::queue<unsigned, 4>> get_queue_for_test_run()
{
    return std::make_unique<jungles::queue_impl<unsigned, 4>>();
}
