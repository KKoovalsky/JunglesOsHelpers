/**
 * @file        thread_pool.hpp
 * @brief       A thread pool.
 */
#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <array>
#include <functional>
#include <mutex>

namespace os::generic
{

struct RunnersCount
{
    constexpr RunnersCount(unsigned count) : count(count)
    {
    }

    const unsigned count;
};

template<RunnersCount RunnersCount_, typename Thread, template<typename> typename Queue, typename Mutex>
struct thread_pool
{
    using Task = std::function<void(void)>;

  private:
    using MessagePump = Queue<Task>;

    static constexpr unsigned runners_count{RunnersCount_.count};

  public:
    thread_pool()
    {
        for (unsigned i = 0; i < runners_count; ++i)
        {
            runners[i].start([&pump = this->pumps[i], &done = this->dones[i]]() {
                while (not done)
                {
                    auto task{pump.receive()};
                    task();
                }
            });
        }
    }

    ~thread_pool()
    {
        for (unsigned i = 0; i < runners_count; ++i)
        {
            std::lock_guard g{mux};
            pumps[i].send([&done = this->dones[i]]() { done = true; });
        }

        for (unsigned i = 0; i < runners_count; ++i)
            runners[i].join();
    }

    void execute(Task task)
    {
        std::lock_guard g{mux};
        pumps[current_runner_idx].send(std::move(task));
        current_runner_idx = (current_runner_idx + 1) % runners_count;
    }

  private:
    std::array<Thread, runners_count> runners;
    std::array<MessagePump, runners_count> pumps;
    std::array<bool, runners_count> dones = {};
    Mutex mux;
    unsigned current_runner_idx{0};
};

} // namespace os::generic

#endif /* THREAD_POOL_HPP */
