/**
 * @file        test_thread_pool.cpp
 * @brief       Tests the thread pool.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <chrono>
#include <mutex>
#include <string>

#include "flag_under_test_definition.hpp"
#include "mutex_under_test.hpp"
#include "thread_pool_under_test.hpp"

#include "platform_utils.hpp"

TEST_CASE("Thread pool executes tasks", "[ThreadPool]") // NOLINT
{
    auto pool{test::make_thread_pool<os::generic::RunnersCount{4}>()};

    SECTION("Executes a task")
    {
        auto flag{test::make_flag()};
        pool.execute([&flag]() { flag.set(); });
        auto is_done{flag.wait_for(std::chrono::seconds{1})};
        REQUIRE(is_done);
    }

    SECTION("Executes multiple tasks")
    {
        auto flag1{test::make_flag()}, flag2{test::make_flag()}, flag3{test::make_flag()};

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
            auto flag1{test::make_flag()}, flag2{test::make_flag()}, flag3{test::make_flag()};

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
            auto flag{test::make_flag()};
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

    SECTION("Executes multiple tasks in parallel")
    {
        REQUIRE(pool.runners_count >= 4);

        constexpr unsigned NumIterations{20};

        std::vector<int> values;
        values.reserve(256);
        auto mux{test::make_mutex()};

        {
            auto flag1{test::make_flag()}, flag2{test::make_flag()}, flag3{test::make_flag()}, flag4{test::make_flag()};

            auto push{[&values, &mux](int v) {
                std::lock_guard g{mux};
                values.push_back(v);
            }};

            auto pusher{[&push](int v) {
                for (unsigned i{0}; i < NumIterations; ++i)
                {
                    push(v);
                    utils::delay(std::chrono::milliseconds{1});
                }
            }};

            pool.execute([&]() {
                pusher(1);
                flag1.set();
            });
            pool.execute([&]() {
                pusher(2);
                flag2.set();
            });
            pool.execute([&]() {
                pusher(3);
                flag3.set();
            });
            pool.execute([&]() {
                pusher(4);
                flag4.set();
            });

            REQUIRE(flag1.wait_for(std::chrono::seconds{1}));
            REQUIRE(flag2.wait_for(std::chrono::seconds{1}));
            REQUIRE(flag3.wait_for(std::chrono::seconds{1}));
            REQUIRE(flag4.wait_for(std::chrono::seconds{1}));
        }

        std::vector<int> values_beginning(std::begin(values), std::next(std::begin(values), NumIterations));

        auto b{std::begin(values_beginning)};
        auto e{std::end(values_beginning)};
        auto is_unique{std::equal(std::next(b), e, b)};
        REQUIRE(not is_unique);
    }
}
