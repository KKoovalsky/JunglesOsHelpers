/**
 * @file        test_event_group.cpp
 * @brief       Event group tests.
 */
#include <catch2/catch_test_macros.hpp>

#include "../event_enum.hpp"
#include "jungles_os_helpers/freertos/event_group.hpp"

#include "flag_under_test_definition.hpp"
#include "thread_under_test_definition.hpp"

using namespace jungles::freertos;
using namespace test_helpers;

using EventGroup32 = event_group<Event::e1,
                                 Event::e2,
                                 Event::e3,
                                 Event::e4,
                                 Event::e5,
                                 Event::e6,
                                 Event::e7,
                                 Event::e8,
                                 Event::e9,
                                 Event::e10,
                                 Event::e11,
                                 Event::e12,
                                 Event::e13,
                                 Event::e14,
                                 Event::e15,
                                 Event::e16,
                                 Event::e17,
                                 Event::e18,
                                 Event::e19,
                                 Event::e20,
                                 Event::e21,
                                 Event::e22,
                                 Event::e23,
                                 Event::e24>;

TEST_CASE("Event groups are set", "[EventGroup][EventGroupSet]")
{
    EventGroup32 eg;

    SECTION("Zero by default")
    {
        REQUIRE(eg.get() == 0);
    }

    SECTION("Sets multiple bits, one by one")
    {
        eg.set<Event::e21>();
        REQUIRE(eg.get() == 0b100000000000000000000);

        eg.set<Event::e2>();
        REQUIRE(eg.get() == 0b100000000000000000010);

        eg.set<Event::e15>();
        REQUIRE(eg.get() == 0b100000100000000000010);
    }

    SECTION("Sets multiple bits at once")
    {
        eg.set<Event::e21, Event::e1, Event::e9, Event::e17>();
        REQUIRE(eg.get() == 0b00000000100010000000100000001);

        eg.set<Event::e11, Event::e7, Event::e1, Event::e13, Event::e22>();
        REQUIRE(eg.get() == 0b00000001100010001010101000001);
    }
}

TEST_CASE("Event groups are cleared", "[EventGroup][EventGroupClear]")
{
    EventGroup32 eg;
    eg.set<Event::e1,
           Event::e3,
           Event::e5,
           Event::e7,
           Event::e9,
           Event::e11,
           Event::e13,
           Event::e15,
           Event::e17,
           Event::e19,
           Event::e21,
           Event::e23>();

    SECTION("Clears multiple bits, one by one")
    {
        eg.clear<Event::e21>();
        REQUIRE(eg.get() == 0b010001010101010101010101);

        eg.clear<Event::e2>();
        REQUIRE(eg.get() == 0b010001010101010101010101);

        eg.clear<Event::e13>();
        REQUIRE(eg.get() == 0b010001010100010101010101);
    }

    SECTION("Clears multiple bits at once")
    {
        eg.clear<Event::e21, Event::e1, Event::e9, Event::e16>();
        REQUIRE(eg.get() == 0b010001010101010001010100);

        eg.clear<Event::e11, Event::e7, Event::e1, Event::e3, Event::e22>();
        REQUIRE(eg.get() == 0b010001010101000000010000);
    }
}

TEST_CASE("Event groups are awaited", "[EventGroup][EventGroupWait]")
{
    EventGroup32 eg;

    SECTION("Waits for one event")
    {
        auto is_thread_started_flag{get_flag_implementation_under_test()};
        auto is_event_set_flag{get_flag_implementation_under_test()};

        Event captured_event = {};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            is_thread_started_flag.set();
            captured_event = eg.wait_one<Event::e22>();
            is_event_set_flag.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            is_thread_started_flag.wait_for(2000);
            eg.set<Event::e22>();
        });

        auto is_event_set{is_event_set_flag.wait_for(3000)};
        REQUIRE(is_event_set);
        REQUIRE(captured_event == Event::e22);

        tsetter.join();
        twaiter.join();
    }

    SECTION("Waits for multiple events")
    {
        auto sync1{get_flag_implementation_under_test()};
        auto sync2{get_flag_implementation_under_test()};
        auto sync3{get_flag_implementation_under_test()};
        auto is_event_set_flag{get_flag_implementation_under_test()};

        Event captured_event1 = {};
        Event captured_event2 = {};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.set();
            captured_event1 = eg.wait_one<Event::e2>();
            sync2.set();
            captured_event2 = eg.wait_one<Event::e24>();
            sync3.set();
            is_event_set_flag.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            sync1.wait_for(2000);
            eg.set<Event::e2>();
            sync2.wait_for(2000);
            eg.set<Event::e24>();
            sync3.wait_for(2000);
        });

        auto is_event_set{is_event_set_flag.wait_for(5000)};
        REQUIRE(is_event_set);
        REQUIRE(captured_event1 == Event::e2);
        REQUIRE(captured_event2 == Event::e24);

        tsetter.join();
        twaiter.join();
    }
}
