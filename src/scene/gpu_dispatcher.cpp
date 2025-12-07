#include "gpu_dispatcher.h"

namespace wow::scene {
    void gpu_dispatcher::dispatch(work_item_t item) {
        std::lock_guard lock{_work_lock};
        _work_queue.push(std::move(item));
        _has_work.store(true, std::memory_order::release);
    }

    void gpu_dispatcher::process_one_frame() {
        const auto start_time = std::chrono::steady_clock::now();
        int32_t work_count = 0;
        while (_has_work.load(std::memory_order::acquire)) {
            work_item_t item{};
            {
                std::lock_guard lock{_work_lock};
                if (!_work_queue.empty()) {
                    item = std::move(_work_queue.front());
                    _work_queue.pop();
                }

                if (_work_queue.empty()) {
                    _has_work.store(false, std::memory_order_release);
                }
            }

            if (item) {
                item();
                ++work_count;
            }

            if (work_count > 25) {
                return;
            }

            if (const auto current_time = std::chrono::steady_clock::now();
                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() > 10) {
                return;
            }
        }
    }
}
