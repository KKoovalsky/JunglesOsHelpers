/**
 * @file	flag.hpp
 * @brief	Defines an inteface for a flag.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef FLAG_HPP
#define FLAG_HPP

namespace jungles
{

//! An interface for a flag.
struct flag
{
    virtual void set() = 0;
    virtual void reset() = 0;
    virtual void wait() = 0;
    virtual bool is_set() const = 0;
    virtual ~flag() = default;
};

}; // namespace jungles

#endif /* FLAG_HPP */
