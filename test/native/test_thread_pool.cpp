/**
 * @file        test_thread_pool.cpp
 * @brief       Tests the thread pool.
 */
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>
#include <thread>

#include "jungles_os_helpers/generic/thread_pool.hpp"

#include "jungles_os_helpers/native/flag.hpp"
#include "jungles_os_helpers/native/message_pump.hpp"
#include "jungles_os_helpers/native/thread.hpp"

#include "platform_utils.hpp"

using namespace os::generic;
using Flag = jungles::flag;

TEST_CASE("Thread pool executes tasks", "[ThreadPool]") // NOLINT
{
    thread_pool<RunnersCount{4}, jungles::native::thread, jungles::native::message_pump, std::mutex> pool;

    SECTION("Executes a task")
    {
        Flag flag;
        pool.execute([&flag]() { flag.set(); });
        auto is_done{flag.wait_for(std::chrono::seconds{1})};
        REQUIRE(is_done);
    }

    SECTION("Executes multiple tasks")
    {
        Flag flag1, flag2, flag3;

        pool.execute([&]() { flag1.set(); });
        pool.execute([&]() { flag2.set(); });
        pool.execute([&]() { flag3.set(); });

        auto is_done1{flag1.wait_for(std::chrono::seconds{1})};
        auto is_done2{flag2.wait_for(std::chrono::seconds{1})};
        auto is_done3{flag3.wait_for(std::chrono::seconds{1})};

        REQUIRE(is_done1);
        REQUIRE(is_done2);
        REQUIRE(is_done3);
    }

    SECTION("Executes multiple tasks in the background")
    {
        constexpr unsigned NumRuns{100};
        constexpr unsigned ExpectedSuccessRatioPercent{90};
        constexpr unsigned ExpectedSuccessfulRuns{NumRuns * ExpectedSuccessRatioPercent / 100};
        constexpr unsigned MaxFailedRuns{NumRuns - ExpectedSuccessfulRuns};

        unsigned failed_runs{0};

        for (unsigned i = 0; i < NumRuns; ++i)
        {
            Flag flag1, flag2, flag3;

            pool.execute([&]() {
                utils::delay(std::chrono::milliseconds{1});
                flag1.set();
            });
            pool.execute([&]() {
                utils::delay(std::chrono::milliseconds{1});
                flag2.set();
            });
            pool.execute([&]() {
                utils::delay(std::chrono::milliseconds{1});
                flag3.set();
            });

            if (flag1.is_set() or flag2.is_set() or flag3.is_set())
                if (++failed_runs >= MaxFailedRuns)
                    FAIL("Tasks are not run in the background, num runs: " << i);

            auto is_done1{flag1.wait_for(std::chrono::seconds{1})};
            auto is_done2{flag2.wait_for(std::chrono::seconds{1})};
            auto is_done3{flag3.wait_for(std::chrono::seconds{1})};

            REQUIRE(is_done1);
            REQUIRE(is_done2);
            REQUIRE(is_done3);
        }
    }

    SECTION("Captures moved local objects")
    {
        constexpr unsigned NumRuns{100};
        constexpr unsigned ExpectedSuccessRatioPercent{90};
        constexpr unsigned ExpectedSuccessfulRuns{NumRuns * ExpectedSuccessRatioPercent / 100};
        constexpr unsigned MaxFailedRuns{NumRuns - ExpectedSuccessfulRuns};

        unsigned failed_runs{0};
        for (unsigned i = 0; i < NumRuns; ++i)
        {
            Flag flag;
            std::string captured_string;

            {
                std::string s{"MAKAPAKA SZASTA FASTA"};
                pool.execute([&flag, &captured_string, str = std::move(s)]() {
                    utils::delay(std::chrono::milliseconds{1});
                    captured_string = str;
                    flag.set();
                });
                REQUIRE(s.empty());
            }

            if (flag.is_set())
                if (++failed_runs >= MaxFailedRuns)
                    FAIL("Cannot capture local object in the background, num runs: " << i);

            auto is_done{flag.wait_for(std::chrono::seconds{1})};
            REQUIRE(is_done);
            REQUIRE(captured_string == "MAKAPAKA SZASTA FASTA");
        }
    }
}
