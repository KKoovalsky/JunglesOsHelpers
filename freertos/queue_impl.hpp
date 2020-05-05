/**
 * @file	queue_impl.hpp
 * @brief	Implements queue basing on the interface in queue.hpp for FreeRTOS.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef QUEUE_IMPL_HPP
#define QUEUE_IMPL_HPP

#include "projdefs.h"
#include "queue.hpp"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "lockguard.hpp"

#include <array>
#include <tl/expected.hpp>

#include <boost/container/static_vector.hpp>

namespace jungles
{

template<typename ElementType, std::size_t Size>
class queue_impl : public queue<ElementType>
{
  public:
    static_assert(Size > 1, "This implementation will not work with size smaller than 2");

    queue_impl(const queue_impl&) = delete;
    queue_impl& operator=(const queue_impl&) = delete;
    queue_impl(queue_impl&&) = delete;
    queue_impl& operator=(queue_impl&&) = delete;

    queue_impl() :
        queue_depot_mux{xSemaphoreCreateMutexStatic(&queue_depot_mux_storage)},
        num_elements_counting_sem{xSemaphoreCreateCountingStatic(Size, 0, &num_elements_counting_sem_storage)}
    {
        assert(queue_depot_mux != nullptr);
        assert(num_elements_counting_sem != nullptr);
    }

    ~queue_impl()
    {
        vSemaphoreDelete(queue_depot_mux);
        vSemaphoreDelete(num_elements_counting_sem);
    }

    virtual os_error send(ElementType&& elem) override
    {
        bool is_full{false};
        {
            lockguard g(queue_depot_mux);
            is_full = queue_depot_elem_count == Size;
            if (!is_full)
            {
                queue_depot.insert(iterator_from_index(queue_depot_head), std::move(elem));
                increment_circular_buffer_index(queue_depot_head);
                ++queue_depot_elem_count;
            }
        }
        if (!is_full) { assert(xSemaphoreGive(num_elements_counting_sem) == pdTRUE); }
        return is_full ? os_error::queue_full : os_error::ok;
    }

    virtual tl::expected<ElementType, os_error> receive(unsigned timeout_ms)
    {
        if (xSemaphoreTake(num_elements_counting_sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE)
        {
            lockguard g(queue_depot_mux);
            auto r{std::move(queue_depot[queue_depot_tail])};
            increment_circular_buffer_index(queue_depot_tail);
            --queue_depot_elem_count;
            return r;
        } else
        {
            return tl::unexpected(os_error::queue_empty);
        }
    }

  private:
    void increment_circular_buffer_index(unsigned& index)
    {
        index = (index + 1) % Size;
    }

    auto iterator_from_index(unsigned index)
    {
        return std::next(std::begin(queue_depot), index);
    }

    //! @todo Shall be circular buffer.
    boost::container::static_vector<ElementType, Size> queue_depot;
    unsigned queue_depot_tail{0}, queue_depot_head{0}, queue_depot_elem_count{0};
    SemaphoreHandle_t queue_depot_mux;
    SemaphoreHandle_t num_elements_counting_sem;
    StaticSemaphore_t queue_depot_mux_storage;
    StaticSemaphore_t num_elements_counting_sem_storage;

}; // namespace jungles

} // namespace jungles

#endif /* QUEUE_IMPL_HPP */
