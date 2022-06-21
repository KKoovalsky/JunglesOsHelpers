/**
 * @file test_poller.cpp
 * @author Kacper Kowalski (kacper.s.kowalski@gmail.com)
 * @brief Tests whether the poller finishes when timeout occurs or when predicate returns true.
 * @date 2020-07-31
 */
#include "catch2/catch_test_macros.hpp"

#include "jungles_os_helpers/generic/poller2.hpp"

using namespace jungles;

TEST_CASE("Poller polls a predicate", "[poller2]")
{
    auto make_predicate_being_false_until_call_number{[](int num_false_calls) {
        return [=, num_called = 0]() mutable {
            ++num_called;
            return num_called >= num_false_calls;
        };
    }};

    auto delay{[](DelayMilliseconds) {
    }};

    auto poll{jungles::generic::poll<delay>};

    SECTION("Poller finishes immediately after predicate returns true on the first polling")
    {
        auto result{poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, []() { return true; })};
        REQUIRE(result == true);
    }

    SECTION("Poller finishes immediately after predicate returns true NOT on the first polling")
    {
        auto result{poll(
            PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, make_predicate_being_false_until_call_number(3))};
        REQUIRE(result == true);
    }

    SECTION("Delayer is not called if first polling returns success")
    {
        static bool is_called{false};
        auto delay{[](DelayMilliseconds) {
            is_called = true;
        }};

        auto poll{jungles::generic::poll<delay>};
        poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, []() { return true; });

        REQUIRE(is_called == false);
    }

    SECTION("Delayer is called after predicate is false")
    {
        static bool is_delayer_called{false};
        auto delay{[](DelayMilliseconds) {
            is_delayer_called = true;
        }};

        auto poll{jungles::generic::poll<delay>};
        poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, make_predicate_being_false_until_call_number(2));

        REQUIRE(is_delayer_called);
    }

    SECTION("Delayer is called exactly that many times as the predicate is false")
    {
        static unsigned delayer_called_times{0};
        auto delay{[](DelayMilliseconds) {
            ++delayer_called_times;
        }};

        auto poll{jungles::generic::poll<delay>};
        poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, make_predicate_being_false_until_call_number(4));

        REQUIRE(delayer_called_times == 3);
    }

    SECTION("Delay is equal to the delay set for the poller")
    {
        static unsigned actual_delay{0};
        auto delay{[](DelayMilliseconds delay_ms) {
            actual_delay = delay_ms.value;
        }};

        auto poll{jungles::generic::poll<delay>};
        poll(PollIntervalMilliseconds{33}, TimeoutMilliseconds{100}, make_predicate_being_false_until_call_number(2));

        REQUIRE(actual_delay == 33);
    }

    SECTION("Poller fails when timeout occurs")
    {
        SECTION("When the timeout is a multiply of the interval")
        {
            auto result{poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{40}, []() { return false; })};
            REQUIRE(result == false);
        }

        SECTION("When the timeout is NOT a multiply of the interval")
        {
            auto result{poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{41}, []() { return false; })};
            REQUIRE(result == false);
        }
    }

    SECTION("Timeout is obeyed")
    {
        SECTION("When the timeout is a multiply of the interval")
        {
            static unsigned delayer_called_times{0};
            auto delay{[](DelayMilliseconds) {
                ++delayer_called_times;
            }};

            auto poll{jungles::generic::poll<delay>};
            poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{100}, []() { return false; });

            REQUIRE(delayer_called_times == 10);
        }

        SECTION("When the timeout is NOT a multiply of the interval")
        {
            static unsigned delayer_called_times{0};
            auto delay{[](DelayMilliseconds) {
                ++delayer_called_times;
            }};

            auto poll{jungles::generic::poll<delay>};
            poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{101}, []() { return false; });

            REQUIRE(delayer_called_times == 11);
        }

        SECTION("When the timeout is lower than the interval")
        {
            static unsigned delayer_called_times{0};
            auto delay{[](DelayMilliseconds) {
                ++delayer_called_times;
            }};

            auto poll{jungles::generic::poll<delay>};
            poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{5}, []() { return false; });

            REQUIRE(delayer_called_times == 1);
        }

        SECTION("When the timeout is equal the interval")
        {
            static unsigned delayer_called_times{0};
            auto delay{[](DelayMilliseconds) {
                ++delayer_called_times;
            }};

            auto poll{jungles::generic::poll<delay>};
            poll(PollIntervalMilliseconds{10}, TimeoutMilliseconds{10}, []() { return false; });

            REQUIRE(delayer_called_times == 1);
        }
    }
}
