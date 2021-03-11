/**
 * @file	lockguard.hpp
 * @brief	Declaration of FreeRTOS lockguard.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef LOCKGUARD_HPP
#define LOCKGUARD_HPP

#include "FreeRTOS.h"
#include "semphr.h"

#include <cassert>

namespace jungles
{

namespace freertos
{

//! Implements RAII for semaphore/mutex locking.
template<typename LockableObjectHandle>
class lockguard
{
  public:
    lockguard(const lockguard&) = delete;
    lockguard& operator=(const lockguard&) = delete;
    lockguard(lockguard&&) = delete;
    lockguard& operator=(lockguard&&) = delete;

    //! The mutex must be created before this constructor is used.
    lockguard(LockableObjectHandle o) noexcept : lockable{o}
    {
        xSemaphoreTake(lockable, portMAX_DELAY);
    }

    ~lockguard()
    {
        xSemaphoreGive(lockable);
    }

  private:
    LockableObjectHandle lockable;
};

} // namespace freertos

} // namespace jungles

#endif /* LOCKGUARD_HPP */
