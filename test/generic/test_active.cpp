/**
 * @file	test_active.cpp
 * @brief	Generic test for an Active object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch.hpp"
#include "test/test_helpers.hpp"

#include "active_under_test_definition.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

TEST_CASE("Active object performs work in background", "[active]")
{
    auto make_callback{[](bool& touched) {
        return [&touched]() {
            touched = true;
        };
    }};

    SECTION("Few messages are handled by the active object in the background")
    {
        bool touched1{false}, touched2{false}, touched3{false};
        {
            auto a{
                get_active_object_for_test_run<test_helpers::message>([](test_helpers::message&& m) { m.callback(); })};
            a.send(test_helpers::message{make_callback(touched1)});
            a.send(test_helpers::message{make_callback(touched2)});
            a.send(test_helpers::message{make_callback(touched3)});
        }
        REQUIRE(touched1 == true);
        REQUIRE(touched2 == true);
        REQUIRE(touched3 == true);
    }
}
