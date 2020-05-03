/**
 * @file	test_active.cpp
 * @brief	Generic test for an Active object.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch.hpp"
#include "test/test_helpers.hpp"

#include "active.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

using namespace jungles;

//! Shall return active_impl basing on the compilation target (for FreeRTOS or e.g. mbed).
std::unique_ptr<active<test_helpers::message>>
    get_active_object_for_test_run(typename active<test_helpers::message>::on_message_received_handler);

TEST_CASE("Active object performs work in background", "[active]")
{
    auto make_callback{[](bool& touched) { return [&touched]() { touched = true; }; }};

    SECTION("Few messages are handled by the active object in the background")
    {
        bool touched1{false}, touched2{false}, touched3{false};
        {
            std::unique_ptr<active<test_helpers::message>> a{
                get_active_object_for_test_run([](test_helpers::message&& m) { m.callback(); })};
            a->send(test_helpers::message{make_callback(touched1)});
            a->send(test_helpers::message{make_callback(touched2)});
            a->send(test_helpers::message{make_callback(touched3)});
        }
        REQUIRE(touched1 == true);
        REQUIRE(touched2 == true);
        REQUIRE(touched3 == true);
    }
}
