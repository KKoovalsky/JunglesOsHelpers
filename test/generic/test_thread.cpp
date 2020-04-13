/**
 * @file	test_thread.cpp
 * @brief	Generic test for a thread object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */

#include "catch2/catch.hpp"

#include "thread.hpp"

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>

using namespace jungles;

//! Shall return thread implementation basing on the compilation target (for FreeRTOS or e.g. mbed).
std::unique_ptr<thread> get_thread_for_test_run(std::function<void(void)>);

struct flag
{
    std::mutex mux;
    std::condition_variable cv;
    bool finished{false};

    void set()
    {
        {
            std::lock_guard g{mux};
            finished = true;
        }
        cv.notify_all();
    }

    void wait()
    {
        std::unique_lock lk{mux};
        cv.wait(lk, [&] { return finished; });
    }

    bool is_set() const
    {
        return finished;
    }
};

struct notify_on_destruction
{
    flag& f;

    explicit notify_on_destruction(flag& f) : f{f} {}

    ~notify_on_destruction()
    {
        f.set();
    }
};

TEST_CASE("Thread is able to run a task", "[active]")
{
    SECTION("Thread blocks, finishes and joins on destruction")
    {
        flag thread_started_flag;
        flag thread_shall_finish;

        {
            auto t{get_thread_for_test_run([&]() {
                thread_started_flag.set();
                thread_shall_finish.wait();
            })};

            notify_on_destruction n{thread_shall_finish};

            thread_started_flag.wait();
        }

        REQUIRE(thread_started_flag.is_set() == true);
        REQUIRE(thread_shall_finish.is_set() == true);
    }

    SECTION("Thread continues to work after detaching")
    {
        flag thread_destructed_flag;
        flag thread_shall_set_this_flag_after_detach;

        {
            auto t{get_thread_for_test_run([&]() {
                thread_destructed_flag.wait();
                thread_shall_set_this_flag_after_detach.set();
            })};
            REQUIRE(t->detach() == os_error::ok);
        }

        thread_destructed_flag.set();
        thread_shall_set_this_flag_after_detach.wait();

        REQUIRE(thread_destructed_flag.is_set() == true);
        REQUIRE(thread_shall_set_this_flag_after_detach.is_set() == true);
    }

    SECTION("Thread blocks, finishes and joins on explicit call for joining")
    {
        flag thread_started_flag;
        flag thread_shall_finish;

        {
            auto t{get_thread_for_test_run([&]() {
                thread_started_flag.set();
                thread_shall_finish.wait();
            })};

            thread_started_flag.wait();
            thread_shall_finish.set();

            REQUIRE(t->join() == os_error::ok);
        }

        REQUIRE(thread_started_flag.is_set() == true);
        REQUIRE(thread_shall_finish.is_set() == true);
    }

    SECTION("Can't detach after joining")
    {
        auto t{get_thread_for_test_run([]() {})};
        t->join();
        REQUIRE(t->detach() != os_error::ok);
    }

    SECTION("Can't detach after detach")
    {
        auto t{get_thread_for_test_run([]() {})};
        t->detach();
        REQUIRE(t->detach() != os_error::ok);
    }

    SECTION("Can join multiple times")
    {
        auto t{get_thread_for_test_run([]() {})};
        t->join();
        REQUIRE(t->join() == os_error::ok);
    }
}
