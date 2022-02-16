/**
 * @file        test_lockable.cpp
 * @brief       Tests lockable concepts.
 * @author      Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include <catch2/catch_test_macros.hpp>

#include "jungles_os_helpers/generic/lockable.hpp"

TEST_CASE("Classes are considered lockable", "[lockable]")
{
    using namespace jungles::generic;

    SECTION("Mutex without try_lock() is BasicLockable, but not Lockable")
    {
        struct Mutex
        {
            void lock()
            {
            }

            void unlock()
            {
            }
        };

        REQUIRE(BasicLockable<Mutex>);
        REQUIRE(not Lockable<Mutex>);
    }

    SECTION("Mutex without unlock() is not BasicLockable")
    {
        struct Mutex
        {
            void lock()
            {
            }
        };

        REQUIRE(not BasicLockable<Mutex>);
    }

    SECTION("Mutex with lock(), unlock() and try_lock() is Lockable and BasicLockable")
    {
        struct Mutex
        {
            void lock()
            {
            }

            void unlock()
            {
            }

            bool try_lock()
            {
                return true;
            }
        };

        REQUIRE(BasicLockable<Mutex>);
        REQUIRE(Lockable<Mutex>);
    }

    SECTION("Mutex without lock() and unlock() but with try_lock() is neither Lockable nor BasicLockable")
    {
        struct Mutex
        {
            bool try_lock()
            {
                return false;
            }
        };

        REQUIRE(not BasicLockable<Mutex>);
        REQUIRE(not Lockable<Mutex>);
    }

    SECTION("Empty struct is neither Lockable nor BasicLockable")
    {
        struct Mutex
        {
        };

        REQUIRE(not BasicLockable<Mutex>);
        REQUIRE(not Lockable<Mutex>);
    }

    SECTION("try_lock() can't return void")
    {
        struct Mutex
        {
            void lock()
            {
            }

            void unlock()
            {
            }

            void try_lock()
            {
            }
        };

        REQUIRE(not Lockable<Mutex>);
    }

    SECTION("try_lock() can't return non-boolean")
    {
        struct Mutex
        {
            void lock()
            {
            }

            void unlock()
            {
            }

            unsigned try_lock()
            {
                return 0;
            }
        };

        REQUIRE(not Lockable<Mutex>);
    }
}
