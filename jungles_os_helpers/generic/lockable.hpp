/**
 * @file        lockable.hpp
 * @brief       Defines lockable concepts.
 * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef LOCKABLE_HPP
#define LOCKABLE_HPP

#include <concepts>

namespace jungles
{
namespace generic
{

template<typename T>
concept BasicLockable = requires(T v)
{
    v.lock();
    v.unlock();
};

template<typename T>
concept Lockable = BasicLockable<T> and requires(T v)
{
    {
        v.try_lock()
        } -> std::same_as<bool>;
};

} // namespace generic
} // namespace jungles

#endif /* LOCKABLE_HPP */
