/**
 * @file        mutex.hpp
 * @brief       Implements a mutex that works with FreeRTOS
 * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef MUTEX_HPP
#define MUTEX_HPP

#include "FreeRTOS.h"
#include "semphr.h"

namespace jungles
{

namespace freertos
{

class mutex
{
  public:
    mutex() : mux{xSemaphoreCreateMutex()}
    {
    }

    void lock()
    {
        xSemaphoreTake(mux, portMAX_DELAY);
    }

    void unlock()
    {
        xSemaphoreGive(mux);
    }

    bool try_lock()
    {
        return xSemaphoreTake(mux, 0) == pdTRUE;
    }

    ~mutex()
    {
        vSemaphoreDelete(mux);
    }

  private:
    SemaphoreHandle_t mux;
};

} // namespace freertos

} // namespace jungles

#endif /* MUTEX_HPP */
