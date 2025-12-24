#ifndef WOW_UNIX_GPU_DISPATCHER_H
#define WOW_UNIX_GPU_DISPATCHER_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace wow::scene {
    class gpu_dispatcher {
    public:
        using work_item_t = std::function<void()>;

    private:
        std::queue<work_item_t> _work_queue{};
        std::mutex _work_lock{};
        std::atomic_bool _has_work{false};

    public:
        void dispatch(work_item_t item);

        void process_one_frame();
    };

    using gpu_dispatcher_ptr = std::shared_ptr<gpu_dispatcher>;
}

#endif //WOW_UNIX_GPU_DISPATCHER_H