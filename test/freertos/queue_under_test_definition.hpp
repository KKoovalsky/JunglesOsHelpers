/**
 * @file	queue_under_test_definition.hpp
 * @brief	Defines the instance to be injected to the queue generic tests
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "jungles_os_helpers/freertos/queue.hpp"

template<typename Element, std::size_t Size>
inline auto get_queue_object_for_test_run()
{
    return jungles::freertos::queue<Element, Size>{};
}
