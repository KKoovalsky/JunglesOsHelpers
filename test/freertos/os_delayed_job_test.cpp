/**
 * @file	os_delayed_job_test.cpp
 * @brief	Tests for the freertos/os_delayed_job class.
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#include "catch2/catch.hpp"

#include "freertos/os_delayed_job.hpp"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace jungles;

TEST_CASE("Jobs are delayed", "[os_delayed_job]")
{
    SECTION("Job is done when detached immediately and the object goes out of scope")
    {
        std::condition_variable cv;
        std::mutex mux;
        bool callback_called{false};
        auto start_time{std::chrono::steady_clock::now()};
        decltype(start_time) callback_called_time{};

        {
            os_delayed_job j;
            REQUIRE(j.start(500, [&]() {
                {
                    std::lock_guard lg{mux};
                    callback_called = true;
                    callback_called_time = std::chrono::steady_clock::now();
                }
                cv.notify_one();
            }) == os_error::ok);
            REQUIRE(j.detach() == os_error::ok);
        }

        std::unique_lock ul{mux};
        using namespace std::chrono_literals;
        auto wait_succesful{cv.wait_for(ul, 10s, [&]() { return callback_called; })};
        REQUIRE(wait_succesful);
        REQUIRE(callback_called == true);

        auto time_passed_to_callback_called{
            std::chrono::duration<double, std::milli>(callback_called_time - start_time).count()};
        REQUIRE(time_passed_to_callback_called >= Approx(500).margin(1));
    }

    SECTION("Job is done when not detached and destruction is synchronized")
    {
        bool callback_called{false};
        auto start_time{std::chrono::steady_clock::now()};

        {
            os_delayed_job j;
            REQUIRE(j.start(500, [&]() { callback_called = true; }) == os_error::ok);
        }

        auto time_destruction_done{std::chrono::steady_clock::now()};
        auto time_passed_to_destruction_done{
            std::chrono::duration<double, std::milli>(time_destruction_done - start_time).count()};
        REQUIRE(time_passed_to_destruction_done >= Approx(500).margin(1));
    }
}
