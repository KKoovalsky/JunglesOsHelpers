/**
 * @file	test_flag.cpp
 * @brief	Tests the flag implementation.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch_test_macros.hpp"

#include <thread>

#include "flag_under_test_definition.hpp"

TEST_CASE("Flag can be set, reset and waited for", "[flag]")
{
    auto flag_under_test{test::make_flag()};

    SECTION("Thread blocks on wait when the flag is not set")
    {
        bool is_waiting_finished{false};

        std::thread t{[&]() {
            flag_under_test.wait();
            is_waiting_finished = true;
        }};

        auto is_waiting_not_finished_before_setting_it{!is_waiting_finished};
        flag_under_test.set();

        t.join();

        REQUIRE(is_waiting_not_finished_before_setting_it);
        REQUIRE(is_waiting_finished);

        SECTION("And the flag is still set")
        {
            REQUIRE(flag_under_test.is_set());
        }
    }

    SECTION("Thread doesn't block on wait when the flag is set")
    {
        flag_under_test.set();

        auto flag_was_set_at_the_beginning{flag_under_test.is_set()};

        flag_under_test.wait();

        REQUIRE(flag_was_set_at_the_beginning);

        SECTION("And the flag is still set")
        {
            REQUIRE(flag_under_test.is_set());
        }
    }

    SECTION("Flag can be set multiple times")
    {
        flag_under_test.set();
        flag_under_test.set();
        flag_under_test.set();
        flag_under_test.set();
        flag_under_test.set();

        auto flag_was_set_at_the_beginning{flag_under_test.is_set()};
        REQUIRE(flag_was_set_at_the_beginning);
    }

    SECTION("Thread blocks on wait after flag is set and then reset")
    {
        flag_under_test.set();
        flag_under_test.reset();

        bool is_waiting_finished{false};

        std::thread t{[&]() {
            flag_under_test.wait();
            is_waiting_finished = true;
        }};

        auto is_waiting_not_finished_before_setting_it{!is_waiting_finished};
        flag_under_test.set();

        t.join();

        REQUIRE(is_waiting_not_finished_before_setting_it);
        REQUIRE(is_waiting_finished);
    }
}
