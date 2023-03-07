/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * */

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <fmt/format.h>


struct thread_pool
{
    struct task_handler;

    uint64_t index = 0;
    std::vector<std::jthread> threads;
    mutable std::mutex unassigned_mutex;
    std::list<std::pair<uint64_t, std::function<void(void)>>> unassigned_tasks;
    mutable std::mutex joined_mutex;
    std::vector<uint64_t> joined_tasks;
    std::atomic_bool exit_pls = false;


    explicit thread_pool(size_t size) {
        auto task_on_thread = [&]() {
            while (not exit_pls.load()) {
                auto lock = std::unique_lock{unassigned_mutex};
                if (unassigned_tasks.empty()) {
                    lock.unlock();
                    std::this_thread::yield();
                    continue;
                }
                auto task = std::move(unassigned_tasks.front());
                unassigned_tasks.pop_front();
                lock.unlock();
                (task.second)();
                auto lock2 = std::unique_lock{joined_mutex};
                joined_tasks.emplace_back(task.first);
            }
        };
        for (auto i = 0ul; i < size; ++i) {
            threads.emplace_back(task_on_thread);
        }
    }

    auto _is_joined(uint64_t idx) const noexcept {
        auto lock = std::scoped_lock{joined_mutex};
        auto it = std::ranges::find(joined_tasks, idx);
        return it == joined_tasks.end();
    }

    auto _join(uint64_t idx) noexcept {
        while (true) {
            auto lock = std::scoped_lock{joined_mutex};
            auto it = std::ranges::find(joined_tasks, idx);
            if (it != joined_tasks.end()) {
                *it = joined_tasks.back();
                joined_tasks.pop_back();
                return;
            }
            std::this_thread::yield();
        }
    }

    struct task_handler
    {
        thread_pool * _ptr;
        uint64_t _idx;

        task_handler(thread_pool * ptr, uint64_t idx) : _ptr{ptr}, _idx{idx} {}

        [[nodiscard]] auto joined() const { return _ptr->_is_joined(_idx); }
        void join() const { _ptr->_join(_idx); }
    };

    template <typename Fn, typename ...Args>
    auto schedule(Fn && fn, Args &&... args) {
        auto lock = std::unique_lock{unassigned_mutex};
        auto const idx = index++;
        unassigned_tasks.emplace_back(idx,
                                      [fn=std::forward<Fn>(fn), args=std::tuple{std::forward<Args>(args)...}] {
                                          return std::apply(std::move(fn), args);
                                      });
        return task_handler{this, idx};
    }

    ~thread_pool() { exit_pls.store(true); }
};


static inline thread_pool thread_pool{std::thread::hardware_concurrency()};


#endif /* THREAD_POOL_HPP */
