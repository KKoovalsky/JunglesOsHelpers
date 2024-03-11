/**
 * @file        test_event_group.cpp
 * @brief       Event group tests.
 */
#include <catch2/catch_test_macros.hpp>

#include "../event_enum.hpp"
#include "jungles_os_helpers/freertos/event_group.hpp"

using namespace jungles::freertos;
using namespace test_helpers;

using EventGroup32 = event_group<Event::e0,
                                 Event::e1,
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

TEST_CASE("Event groups are set properly", "[EventGroup][EventGroupCreation]")
{
    EventGroup32 eg;

    SECTION("Zero by default")
    {
        REQUIRE(eg.get() == 0);
    }
}
