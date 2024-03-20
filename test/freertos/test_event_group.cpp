/**
 * @file        test_event_group.cpp
 * @brief       Event group tests.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <chrono>
#include <vector>

#include "../event_enum.hpp"
#include "jungles_os_helpers/freertos/event_group.hpp"

#include "flag_under_test_definition.hpp"
#include "platform_utils.hpp"
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
        auto is_thread_started_flag{test::make_flag()};
        auto is_event_set_flag{test::make_flag()};

        Event captured_event = {};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            is_thread_started_flag.set();
            captured_event = eg.wait_one<Event::e22>();
            is_event_set_flag.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            is_thread_started_flag.wait_for(std::chrono::milliseconds{2000});
            eg.set<Event::e22>();
        });

        auto is_event_set{is_event_set_flag.wait_for(std::chrono::milliseconds{3000})};
        REQUIRE(is_event_set);
        REQUIRE(captured_event == Event::e22);

        tsetter.join();
        twaiter.join();
    }

    SECTION("Waits for multiple events")
    {
        auto sync1{test::make_flag()};
        auto sync2{test::make_flag()};
        auto sync3{test::make_flag()};
        auto is_event_set_flag{test::make_flag()};

        Event captured_event1 = {};
        Event captured_event2 = {};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.set();
            captured_event1 = eg.wait_one<Event::e2>();
            sync2.set();
            captured_event2 = eg.wait_one<Event::e24>();
            sync3.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            sync1.wait_for(std::chrono::milliseconds{2000});
            eg.set<Event::e2>();
            sync2.wait_for(std::chrono::milliseconds{2000});
            eg.set<Event::e24>();
            sync3.wait_for(std::chrono::milliseconds{2000});
            is_event_set_flag.set();
        });

        auto is_event_set{is_event_set_flag.wait_for(std::chrono::milliseconds{5000})};
        REQUIRE(is_event_set);
        REQUIRE(captured_event1 == Event::e2);
        REQUIRE(captured_event2 == Event::e24);

        tsetter.join();
        twaiter.join();
    }

    SECTION("Gets each event one by one")
    {
        auto sync1{test::make_flag()};
        auto sync2{test::make_flag()};
        auto sync3{test::make_flag()};
        auto is_event_set_flag{test::make_flag()};

        std::vector<Event> captured_events;
        captured_events.reserve(8);

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.wait_for(std::chrono::milliseconds{2000});
            captured_events.push_back(eg.wait_one<Event::e2, Event::e23, Event::e16>());
            captured_events.push_back(eg.wait_one<Event::e2, Event::e23, Event::e16>());
            captured_events.push_back(eg.wait_one<Event::e2, Event::e23, Event::e16>());
            sync2.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            eg.set<Event::e16>();
            eg.set<Event::e2>();
            eg.set<Event::e23>();
            sync1.set();
            sync2.wait_for(std::chrono::milliseconds{2000});
            is_event_set_flag.set();
        });

        auto is_event_set{is_event_set_flag.wait_for(std::chrono::milliseconds{3000})};
        REQUIRE(is_event_set);
        REQUIRE_THAT(captured_events,
                     Catch::Matchers::UnorderedEquals(std::vector<Event>{Event::e16, Event::e2, Event::e23}));

        tsetter.join();
        twaiter.join();
    }

    SECTION("Gets only the awaited events")
    {
        auto sync1{test::make_flag()};
        auto sync2{test::make_flag()};
        auto sync3{test::make_flag()};
        auto is_event_set_flag{test::make_flag()};

        Event captured_event;
        auto begin{std::chrono::high_resolution_clock::now()};
        auto end{std::chrono::high_resolution_clock::now()};
        bool is_event_captured_prematurely{true};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.wait_for(std::chrono::milliseconds{2000});
            begin = std::chrono::high_resolution_clock::now();
            captured_event = eg.wait_one<Event::e2, Event::e23, Event::e16>();
            end = std::chrono::high_resolution_clock::now();
            sync2.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            eg.set<Event::e9>();
            eg.set<Event::e1>();
            eg.set<Event::e22>();
            sync1.set();
            utils::delay(std::chrono::milliseconds{200});
            is_event_captured_prematurely = sync2.is_set();
            eg.set<Event::e23>();
            sync2.wait_for(std::chrono::milliseconds{2000});
            is_event_set_flag.set();
        });

        auto is_event_set{is_event_set_flag.wait_for(std::chrono::milliseconds{3000})};
        REQUIRE(is_event_set);
        REQUIRE(not is_event_captured_prematurely);

        auto duration_ms{std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()};
        REQUIRE(duration_ms >= 200 - 5);
        REQUIRE(captured_event == Event::e23);

        tsetter.join();
        twaiter.join();
    }
}

TEST_CASE("Event groups are awaited with timeout", "[EventGroup][EventGroupWaitTimeout]")
{
    EventGroup32 eg;

    SECTION("Simple timeout case")
    {
        auto sync{test::make_flag()};

        std::optional<Event> captured_event;

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            captured_event = eg.wait_one<Event::e2, Event::e23, Event::e16>(std::chrono::milliseconds{10});
            sync.set();
        });

        auto is_awaiting_done{sync.wait_for(std::chrono::milliseconds{1000})};
        REQUIRE(is_awaiting_done);
        REQUIRE_FALSE(captured_event.has_value());

        twaiter.join();
    }

    SECTION("Events are set, but not the awaited one")
    {
        auto sync1{test::make_flag()};
        auto sync2{test::make_flag()};
        auto sync3{test::make_flag()};
        auto is_event_set_flag{test::make_flag()};

        std::optional<Event> captured_event;
        auto begin{std::chrono::high_resolution_clock::now()};
        auto end{std::chrono::high_resolution_clock::now()};

        auto twaiter{get_thread_for_test_run()};
        twaiter.start([&]() {
            sync1.wait_for(std::chrono::milliseconds{2000});
            begin = std::chrono::high_resolution_clock::now();
            captured_event = eg.wait_one<Event::e2, Event::e23, Event::e16>(std::chrono::milliseconds{50});
            end = std::chrono::high_resolution_clock::now();
            sync2.set();
        });

        auto tsetter{get_thread_for_test_run()};
        tsetter.start([&]() {
            eg.set<Event::e9>();
            eg.set<Event::e1>();
            eg.set<Event::e22>();
            sync1.set();
            sync2.wait_for(std::chrono::milliseconds{2000});
            is_event_set_flag.set();
        });

        auto is_event_set{is_event_set_flag.wait_for(std::chrono::milliseconds{3000})};
        REQUIRE(is_event_set);
        REQUIRE(not captured_event.has_value());

        auto duration_ms{std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()};
        REQUIRE_THAT(duration_ms, Catch::Matchers::WithinAbs(50, 3));

        tsetter.join();
        twaiter.join();
    }
}
