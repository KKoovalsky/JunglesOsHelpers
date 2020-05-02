/**
 * @file	os_def.hpp
 * @brief	Common defines for the OS components of the library
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef OS_DEF_HPP
#define OS_DEF_HPP

namespace jungles
{

enum class os_error
{
    ok = 0,
    error,
    creation_failed,
    start_failed,
    queue_full,
    queue_empty
};

}

#endif /* OS_DEF_HPP */
