/**
 * @file	lockguard.hpp
 * @brief	Declaration of FreeRTOS lockguard.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef LOCKGUARD_HPP
#define LOCKGUARD_HPP

#include "os_def.hpp"

#include "FreeRTOS.h"
#include "portmacro.h"
#include "semphr.h"

#include <cassert>

namespace jungles
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
        assert(xSemaphoreTake(lockable, portMAX_DELAY));
    }

    ~lockguard()
    {
        assert(xSemaphoreGive(lockable));
    }

  private:
    LockableObjectHandle lockable;
};

} // namespace jungles

#endif /* LOCKGUARD_HPP */
