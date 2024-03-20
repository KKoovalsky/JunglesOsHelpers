/**
 * @file        test_simple_thread.cpp
 * @brief       Tests the simple_thread.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include <chrono>
#include <functional>
#include <stdexcept>

#include "flag_under_test_definition.hpp"
#include "platform_utils.hpp"

struct simple_thread
{
    struct Error : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    void start(std::function<void(void)>)
    {
    }
    void join()
    {
    }
};

TEST_CASE("Thread runs in background", "[SimpleThread][SimpleThreadRuns]")
{
    SECTION("Runs in the background")
    {
        auto thread_started_flag{test::make_flag()};
        auto sync{test::make_flag()};
        auto thread_finished_flag{test::make_flag()};

        simple_thread t;
        t.start([&]() {
            thread_started_flag.set();
            sync.wait();
            thread_finished_flag.set();
        });

        REQUIRE(thread_started_flag.wait_for(std::chrono::seconds{1}));
        sync.set();
        REQUIRE(thread_finished_flag.wait_for(std::chrono::seconds{1}));
        t.join();
    }

    SECTION("Can be re-run")
    {
        simple_thread t;
        t.start([]() {});
        t.join();
        t.start([]() {});
        t.join();
        t.start([]() {});
        t.join();

        auto flag{test::make_flag()};
        t.start([&flag]() { flag.set(); });
        t.join();

        auto is_done{flag.wait_for(std::chrono::seconds{1})};
        REQUIRE(is_done);
    }
}

TEST_CASE("Simple thread negative cases", "[SimpleThread][SimpleThreadNegativeCases]")
{
    SECTION("Re-starting before joining is an error")
    {
        auto flag{test::make_flag()};
        simple_thread t;
        t.start([&flag]() { flag.wait(); });

        REQUIRE_THROWS_AS(t.start([]() {}), simple_thread::Error);
        REQUIRE_THROWS_WITH(t.start([]() {}), "Thread is already running");

        flag.set();
        t.join();
    }

    SECTION("Joining before starting is an error")
    {
        simple_thread t;
        REQUIRE_THROWS_AS(t.join(), simple_thread::Error);
        REQUIRE_THROWS_WITH(t.join(), "Thread is not running");
    }

    SECTION("Joining twice is an error")
    {
        simple_thread t;
        t.start([]() {});
        t.join();
        REQUIRE_THROWS_AS(t.join(), simple_thread::Error);
        REQUIRE_THROWS_WITH(t.join(), "Thread is not running");
    }

    SECTION("Destructing before joining is an error")
    {
        auto the_test{[]() {
            auto flag{test::make_flag()};
            simple_thread t;
            t.start([&flag]() {
                // Never set
                flag.wait();
            });
        }};

        REQUIRE_THROWS_AS(the_test(), simple_thread::Error);
        REQUIRE_THROWS_WITH(the_test(), "Thread is still running");
    }
}
