/**
 * @file	queue_sending_from_isr.hpp
 * @brief	Implements queue which sends from ISR context only (won't work from task).
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FREERTOS_QUEUE_SENDING_FROM_ISR_HPP
#define FREERTOS_QUEUE_SENDING_FROM_ISR_HPP

#include "queue.hpp"

namespace jungles
{

namespace freertos
{

/**
 * @brief Sends messages from ISR context.
 *
 * This changes jungles::freertos::queue behavior in such a way, that when send() is called it is performed with
 * FreeRTOS API which can only be used from ISR context. The primary queue implementation can only be used from task
 * context.
 *
 */
template<typename ElementType, std::size_t Size>
class queue_sending_from_isr : public jungles::freertos::queue<ElementType, Size>
{
  public:
    void send(ElementType&& elem)
    {
        using BaseQueue = jungles::freertos::queue<ElementType, Size>;
        BaseQueue::send_from_isr(std::move(elem));
    }
};

} // namespace freertos

} // namespace jungles

#endif /* FREERTOS_QUEUE_SENDING_FROM_ISR_HPP */
