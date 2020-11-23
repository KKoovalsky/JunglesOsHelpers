/**
 * @file	test_queue.hpp
 * @brief	Generic test of queue with pluggable queue implementation to be tested.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch.hpp"

#include <future>
#include <iostream>
#include <thread>

#include "queue_under_test_definition.hpp"
#include "thread_under_test_definition.hpp"

#include "../test_helpers.hpp"

TEST_CASE("Messages are received and sent to the queue", "[queue]")
{
    auto q{get_queue_object_for_test_run<std::string, 4>()};

    SECTION("Few messages sent and received from queue")
    {
        SECTION("All messages sent and then received")
        {
            REQUIRE_NOTHROW(q.send("1"));
            REQUIRE(q.receive(0).value() == "1");
            REQUIRE_NOTHROW(q.send("2"));
            REQUIRE(q.receive(0).value() == "2");
            REQUIRE_NOTHROW(q.send("3"));
            REQUIRE(q.receive(0).value() == "3");
        }
        SECTION("Sending and receiving is interleaved")
        {
            q.send("1");
            q.send("2");
            q.send("3");
            REQUIRE(q.receive(0).value() == "1");
            REQUIRE(q.receive(0).value() == "2");
            REQUIRE(q.receive(0).value() == "3");
        }
    }
    SECTION("Timeout occured when receiving from an empty queue")
    {
        REQUIRE(!q.receive(300).has_value());
    }
    SECTION("Queue overflows when too many elements in the queue")
    {
        q.send("1");
        q.send("1");
        q.send("1");
        REQUIRE_NOTHROW(q.send("1"));
        REQUIRE_THROWS(q.send("1"));
    }
    GIVEN("Queue is empty")
    {
        WHEN("Message 1. is received from the queue with big timeout")
        {
            test_helpers::flag receive_blocked;

            std::promise<std::string> receive_result;
            auto future_receive_result{receive_result.get_future()};

            auto t{get_thread_for_test_run()};
            t.start([&] {
                receive_blocked.set();
                auto r{q.receive(10000)};
                receive_result.set_value(*r);
            });

            AND_WHEN("Message 2. is sent in the same time")
            {
                receive_blocked.wait();
                std::this_thread::yield();
                q.send("5");

                THEN("Message 1. is successfully received")
                {
                    REQUIRE(future_receive_result.get() == "5");
                }
            }
        }
    }
}
