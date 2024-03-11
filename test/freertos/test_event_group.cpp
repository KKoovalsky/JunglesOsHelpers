/**
 * @file        test_event_group.cpp
 * @brief       Event group tests.
 */
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <stdexcept>

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
                                 Event::e24,
                                 Event::e25,
                                 Event::e26,
                                 Event::e27,
                                 Event::e28,
                                 Event::e29,
                                 Event::e30,
                                 Event::e31>;

TEST_CASE("Event groups are set", "[EventGroup][EventGroupSet]")
{
    EventGroup32 eg;

    SECTION("Zero by default")
    {
        REQUIRE(eg.get() == 0);
    }

    SECTION("Sets multiple bits, one by one")
    {
        eg.set(Event::e21);
        REQUIRE(eg.get() == 0b100000000000000000000);

        eg.set(Event::e2);
        REQUIRE(eg.get() == 0b100000000000000000010);

        eg.set(Event::e32);
        REQUIRE(eg.get() == 0b10000000000100000000000000000010);
    }

    SECTION("Sets multiple bits at once")
    {
        eg.set(Event::e21, Event::e1, Event::e29, Event::e17);
        REQUIRE(eg.get() == 0b10000000100010000000000000001);

        eg.set(Event::e11, Event::e7, Event::e1, Event::e30, Event::e22);
        REQUIRE(eg.get() == 0b110000001100010000010001000001);
    }
}

TEST_CASE("Event groups are cleared", "[EventGroup][EventGroupClear]")
{
    EventGroup32 eg;
    eg.set(Event::e1,
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
           Event::e23,
           Event::e25,
           Event::e27,
           Event::e29,
           Event::e31);

    SECTION("Clears multiple bits, one by one")
    {
        eg.clear(Event::e21);
        REQUIRE(eg.get() == 0b01010101010001010101010101010101);

        eg.clear(Event::e2);
        REQUIRE(eg.get() == 0b01010101010001010101010101010101);

        eg.clear(Event::e27);
        REQUIRE(eg.get() == 0b01010001010001010101010101010101);
    }

    SECTION("Clears multiple bits at once")
    {
        eg.clear(Event::e21, Event::e1, Event::e29, Event::e16);
        REQUIRE(eg.get() == 0b01000101010001010101010101010100);

        eg.clear(Event::e11, Event::e7, Event::e1, Event::e31, Event::e22);
        REQUIRE(eg.get() == 0b00000101010001010101000100010100);
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
            captured_event = eg.wait_one(Event::e22);
            is_event_set_flag.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            is_thread_started_flag.wait_for(2000);
            eg.set(Event::e22);
        });

        auto is_event_set{is_event_set_flag.wait_for(3000)};
        CHECK(is_event_set);
        CHECK(captured_event == Event::e22);

        tsetter.join();
        twaiter.join();
    }

    SECTION("Waits for multiple events")
    {
        auto sync1{get_flag_implementation_under_test()};
        auto sync2{get_flag_implementation_under_test()};
        auto is_event_set_flag{get_flag_implementation_under_test()};

        Event captured_event1 = {};
        Event captured_event2 = {};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.set();
            std::cout << "twaiter: HERE0" << std::endl;
            captured_event1 = eg.wait_one(Event::e29);
            std::cout << "twaiter: HERE1" << std::endl;
            sync2.set();
            std::cout << "twaiter: HERE2" << std::endl;
            captured_event2 = eg.wait_one(Event::e2);
            std::cout << "twaiter: HERE3" << std::endl;
            is_event_set_flag.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            if (sync1.wait_for(2000) == false)
                throw std::runtime_error{"sync1 not set"};
            std::cout << "tsetter: HERE2" << std::endl;
            eg.set(Event::e29);
            std::cout << "tsetter: HERE3" << std::endl;
            sync2.wait_for(2000);
            std::cout << "tsetter: HERE4" << std::endl;
            eg.set(Event::e2);
        });

        auto is_event_set{is_event_set_flag.wait_for(3000)};
        CHECK(is_event_set);
        CHECK(captured_event1 == Event::e29);
        CHECK(captured_event1 == Event::e2);

        tsetter.join();
        twaiter.join();
    }
}
