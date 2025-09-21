#include "minimap_loader_pool.h"

namespace wow::io::minimap {
    void minimap_loader_pool::worker_function() {
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

    minimap_loader_pool::minimap_loader_pool() {
        const auto num_threads = std::thread::hardware_concurrency() * 2;
        for (auto i = 0; i < num_threads; ++i) {
            _worker_threads.emplace_back(std::thread{
                [this] {
                    worker_function();
                }
            });
        }
    }

    minimap_loader_pool::~minimap_loader_pool() {
        _running = false;
        _work_cv.notify_all();
        for (auto &thread: _worker_threads) {
            thread.join();
        }
    }

    std::shared_future<void> minimap_loader_pool::submit(const std::function<void()> &task) {
        auto work_item{std::make_shared<std::packaged_task<void()> >(std::move(task))};
        {
            std::lock_guard lock(_work_lock);
            _work_queue.emplace(work_item);
        }
        _work_cv.notify_one();
        return work_item->get_future();
    }
}
