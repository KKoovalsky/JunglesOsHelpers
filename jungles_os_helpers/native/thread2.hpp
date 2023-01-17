/**
 * @file	thread2.hpp
 * @brief	Thread implementation which uses member function as the runner.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef JUNGLES_NATIVE_THREAD2_HPP
#define JUNGLES_NATIVE_THREAD2_HPP

#include <thread>

namespace jungles
{

namespace native
{

namespace detail
{

template<typename T>
struct ClassOf
{
};

template<typename ReturnType, typename Class>
struct ClassOf<ReturnType(Class::*)>
{
    using type = Class;
};

template<typename T>
concept Method = requires(T)
{
    typename ClassOf<T>::type;
};

} // namespace detail

template<auto MethodPointer>
requires detail::Method<decltype(MethodPointer)>
class thread2
{
  public:
    using Owner = typename detail::ClassOf<decltype(MethodPointer)>::type;

    void start(Owner* owner)
    {
        underlying_thread = std::thread{[&]() {
            (owner->*MethodPointer)();
        }};
    }

    void join()
    {
        underlying_thread.join();
    }

  private:
    std::thread underlying_thread;
};

} // namespace native

} // namespace jungles

#endif /* JUNGLES_NATIVE_THREAD2_HPP */
