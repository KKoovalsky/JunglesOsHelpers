/**
 * @file	test_queue.hpp
 * @brief	Generic test of queue with pluggable queue implementation to be tested.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch.hpp"

#include <future>
#include <iostream>
#include <thread>

#include "os_def.hpp"
#include "queue.hpp"
#include "thread.hpp"

#include "../test_helpers.hpp"

static constexpr unsigned queue_size{4};

//! Shall return queue implementation basing on the compilation target (for FreeRTOS or e.g. mbed).
std::unique_ptr<jungles::queue<unsigned, queue_size>> get_queue_for_test_run();

std::unique_ptr<jungles::thread> get_thread_for_test_run(std::function<void(void)>);

TEST_CASE("Messages are received and sent to the queue", "[queue]")
{
    auto q{get_queue_for_test_run()};
    SECTION("Few messages sent and received from queue")
    {
        SECTION("All messages sent and then received")
        {
            REQUIRE(q->send({1}) == jungles::os_error::ok);
            REQUIRE(q->receive(0).value() == 1);
            REQUIRE(q->send({2}) == jungles::os_error::ok);
            REQUIRE(q->receive(0).value() == 2);
            REQUIRE(q->send({3}) == jungles::os_error::ok);
            REQUIRE(q->receive(0).value() == 3);
        }
        SECTION("Sending and receiving is interleaved")
        {
            REQUIRE(q->send({1}) == jungles::os_error::ok);
            REQUIRE(q->send({2}) == jungles::os_error::ok);
            REQUIRE(q->send({3}) == jungles::os_error::ok);
            REQUIRE(q->receive(0).value() == 1);
            REQUIRE(q->receive(0).value() == 2);
            REQUIRE(q->receive(0).value() == 3);
        }
    }
    SECTION("Timeout occured when receiving from an empty queue")
    {
        REQUIRE(q->receive(300).error() == jungles::os_error::queue_empty);
    }
    SECTION("Queue overflows when too many elements in the queue")
    {
        q->send({1});
        q->send({1});
        q->send({1});
        REQUIRE(q->send({1}) == jungles::os_error::ok);
        REQUIRE(q->send({1}) == jungles::os_error::queue_full);
    }
    GIVEN("Queue is empty")
    {
        WHEN("Message 1. is received from the queue with big timeout")
        {
            test_helpers::flag receive_blocked;

            std::promise<unsigned> receive_result;
            auto future_receive_result{receive_result.get_future()};

            auto t{get_thread_for_test_run([&] {
                receive_blocked.set();
                auto r{q->receive(10000)};
                receive_result.set_value(*r);
            })};

            AND_WHEN("Message 2. is sent in the same time")
            {
                receive_blocked.wait();
                std::this_thread::yield();
                q->send({5});

                THEN("Message 1. is successfully received")
                {
                    REQUIRE(future_receive_result.get() == 5);
                }
            }
        }
    }
}
