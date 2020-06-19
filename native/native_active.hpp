/**
 * @file	native_active.hpp
 * @brief	Specializes an Active for a native target.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef NATIVE_ACTIVE_HPP
#define NATIVE_ACTIVE_HPP

#include <thread>

#include "../active_impl.hpp"
#include "../native/flag.hpp"
#include "../native/message_pump.hpp"

namespace jungles
{

template<typename Message>
using native_active = jungles::active_generic_impl<Message, std::thread, jungles::message_pump>;

}

#endif /* NATIVE_ACTIVE_HPP */
