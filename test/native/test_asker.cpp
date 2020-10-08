/**
 * @file test_asker.cpp
 * @author Kacper Kowalski (kacper.s.kowalski@gmail.com)
 * @brief Tests whether the asker finishes when timeout occurs or when predicate returns true.
 * @date 2020-07-31
 */
#include "catch2/catch.hpp"

#include "asker.hpp"

using namespace jungles;

TEST_CASE("Asker asks a predicate", "[asker]")
{
    auto make_predicate_being_false_until_call_number{[](int num_false_calls) {
        return [=, num_called = 0]() mutable {
            ++num_called;
            return num_called >= num_false_calls;
        };
    }};

    SECTION("Asker finishes immediately after predicate returns true on the first asking")
    {
        asker<10, 100> a{[](auto) {
        }};
        auto result{a.ask([]() { return true; })};
        REQUIRE(result == true);
    }

    SECTION("Asker finishes immediately after predicate returns true NOT on the first asking")
    {
        asker<10, 100> a{[&](auto) {
        }};

        auto result{a.ask(make_predicate_being_false_until_call_number(3))};

        REQUIRE(result == true);
    }

    SECTION("Delayer is not called before asking")
    {
        bool delayer_not_called{true};
        asker<10, 100> a{[&](auto) {
            delayer_not_called = false;
        }};
        REQUIRE(delayer_not_called);
    }

    SECTION("Delayer is called after predicate is false")
    {
        bool delayer_called{false};
        asker<10, 100> a{[&](auto) {
            delayer_called = true;
        }};
        a.ask(make_predicate_being_false_until_call_number(2));

        REQUIRE(delayer_called);
    }

    SECTION("Delayer is called exactly that many times as the predicate is false")
    {
        unsigned delayer_called_times{0};
        asker<10, 100> a{[&](auto) {
            ++delayer_called_times;
        }};
        a.ask(make_predicate_being_false_until_call_number(4));

        REQUIRE(delayer_called_times == 3);
    }

    SECTION("Delay is equal to the delay set for the asker")
    {
        unsigned actual_delay{0};
        asker<33, 100> a{[&](auto delay) {
            actual_delay = delay;
        }};
        a.ask(make_predicate_being_false_until_call_number(2));

        REQUIRE(actual_delay == 33);
    }

    SECTION("Asker fails when timeout occurs")
    {
        SECTION("When the timeout is a multiply of the interval")
        {
            asker<10, 40> a{[](auto) {
            }};
            auto result{a.ask([]() { return false; })};

            REQUIRE(result == false);
        }

        SECTION("When the timeout is NOT a multiply of the interval")
        {
            asker<10, 41> a{[](auto) {
            }};
            auto result{a.ask([]() { return false; })};

            REQUIRE(result == false);
        }
    }

    SECTION("Timeout is obeyed")
    {
        SECTION("When the timeout is a multiply of the interval")
        {
            unsigned delayer_called_times{0};
            asker<10, 100> a{[&](auto) {
                ++delayer_called_times;
            }};
            a.ask([]() { return false; });

            REQUIRE(delayer_called_times == 10);
        }

        SECTION("When the timeout is NOT a multiply of the interval")
        {
            unsigned delayer_called_times{0};
            asker<10, 101> a{[&](auto) {
                ++delayer_called_times;
            }};
            a.ask([]() { return false; });

            REQUIRE(delayer_called_times == 11);
        }
    }
}
