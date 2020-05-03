/**
 * @file	test_helpers.hpp
 * @brief	Common test helpers
 * @author	Kacper Kowalski - kacper.s.kowalski@gmail.com
 */
#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include <condition_variable>
#include <functional>
#include <mutex>

namespace test_helpers
{

struct flag
{
    std::mutex mux;
    std::condition_variable cv;
    bool finished{false};

    void set()
    {
        {
            std::lock_guard g{mux};
            finished = true;
        }
        cv.notify_all();
    }

    void wait()
    {
        std::unique_lock lk{mux};
        cv.wait(lk, [&] { return finished; });
    }

    bool is_set() const
    {
        return finished;
    }
};

struct notify_on_destruction
{
    flag& f;

    explicit notify_on_destruction(flag& f) : f{f} {}

    ~notify_on_destruction()
    {
        f.set();
    }
};

struct message
{
    std::function<void(void)> callback;

    explicit message(std::function<void(void)> callback) : callback{callback} {}
};

} // namespace test_helpers

#endif /* TEST_HELPERS_HPP */
