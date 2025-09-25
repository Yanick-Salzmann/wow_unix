#include "work_pool.h"

namespace wow::utils {
    void work_pool::worker_function() {
        while (_running) {
            work_item_ptr work_item{};
            {
                std::unique_lock lock(_work_lock);
                _work_cv.wait(lock, [this] { return !_work_queue.empty() || !_running; });
                if (!_running) {
                    return;
                }
                work_item = std::move(_work_queue.front());
                _work_queue.pop();
            }

            if (work_item) {
                (*work_item)();
            }
        }
    }

    work_pool::work_pool() {
        const auto num_threads = std::max(1u, std::thread::hardware_concurrency());
        for (auto i = 0; i < num_threads; ++i) {
            _worker_threads.emplace_back(std::thread{
                [this] {
                    worker_function();
                }
            });
        }
    }

    work_pool::~work_pool() {
        _running = false;
        _work_cv.notify_all();
        for (auto &thread: _worker_threads) {
            thread.join();
        }
    }

    std::shared_future<void> work_pool::submit(const std::function<void()> &task) {
        auto work_item{std::make_shared<std::packaged_task<void()> >(std::move(task))};
        {
            std::lock_guard lock(_work_lock);
            _work_queue.emplace(work_item);
        }
        _work_cv.notify_one();
        return work_item->get_future();
    }
}