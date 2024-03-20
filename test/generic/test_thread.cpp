/**
 * @file	test_thread.cpp
 * @brief	Generic test for a thread object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "catch2/catch_test_macros.hpp"

#include "../test_helpers.hpp"

#include "thread_under_test_definition.hpp"

TEST_CASE("Thread is able to run a task", "[thread]")
{
    SECTION("Thread blocks, finishes and joins on destruction")
    {
        test_helpers::flag thread_started_flag;
        test_helpers::flag thread_shall_finish;

        {
            auto t{get_thread_for_test_run()};
            t.start([&]() {
                thread_started_flag.set();
                thread_shall_finish.wait();
            });

            test_helpers::notify_on_destruction n{thread_shall_finish};

            thread_started_flag.wait();
        }

        REQUIRE(thread_started_flag.is_set() == true);
        REQUIRE(thread_shall_finish.is_set() == true);
    }

    SECTION("Thread continues to work after detaching")
    {
        test_helpers::flag thread_destructed_flag;
        test_helpers::flag thread_shall_set_this_flag_after_detach;

        {
            auto t{get_thread_for_test_run()};
            t.start([&]() {
                thread_destructed_flag.wait();
                thread_shall_set_this_flag_after_detach.set();
            });
            REQUIRE_NOTHROW(t.detach());
        }

        thread_destructed_flag.set();
        thread_shall_set_this_flag_after_detach.wait();

        REQUIRE(thread_destructed_flag.is_set() == true);
        REQUIRE(thread_shall_set_this_flag_after_detach.is_set() == true);
    }

    SECTION("Thread blocks, finishes and joins on explicit call for joining")
    {
        test_helpers::flag thread_started_flag;
        test_helpers::flag thread_shall_finish;

        {
            auto t{get_thread_for_test_run()};
            t.start([&]() {
                thread_started_flag.set();
                thread_shall_finish.wait();
            });

            thread_started_flag.wait();
            thread_shall_finish.set();

            REQUIRE_NOTHROW(t.join());
        }

        REQUIRE(thread_started_flag.is_set() == true);
        REQUIRE(thread_shall_finish.is_set() == true);
    }

    SECTION("Can't detach after joining")
    {
        auto t{get_thread_for_test_run()};
        t.start([]() {});
        t.join();
        REQUIRE_THROWS(t.detach());
    }

    SECTION("Can't detach after detach")
    {
        // NOTE: this flag shouldn't be needed but I have found some erroneous behavior of the FreeRTOS port that
        // it hangs sometimes when the thread created in the next SECTION is started before the thread created in
        // this section.
        test_helpers::flag thread_finished_flag;
        {
            auto t{get_thread_for_test_run()};
            t.start([&]() { thread_finished_flag.set(); });
            t.detach();
            REQUIRE_THROWS(t.detach());
        }
        thread_finished_flag.wait();
    }

    SECTION("Can join multiple times")
    {
        auto t{get_thread_for_test_run()};
        t.start([]() {});
        t.join();
        REQUIRE_NOTHROW(t.join());
    }

    SECTION("Not started thread is destructed correctly")
    {
        get_thread_for_test_run();
    }
}
