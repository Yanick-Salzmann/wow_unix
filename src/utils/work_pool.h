#ifndef WOW_UNIX_WORK_POOL_H
#define WOW_UNIX_WORK_POOL_H

#include <future>
#include <memory>
#include <queue>

namespace wow::utils {
    class work_pool {
        using work_item_ptr = std::shared_ptr<std::packaged_task<void()> >;

        std::vector<std::thread> _worker_threads{};
        std::mutex _work_lock{};
        std::condition_variable _work_cv{};

        std::queue<work_item_ptr> _work_queue{};

        bool _running = true;

        void worker_function();

    public:
        work_pool();

        ~work_pool();

        std::shared_future<void> submit(const std::function<void()> &task);
    };

    using work_pool_ptr = std::shared_ptr<work_pool>;
};

#endif //WOW_UNIX_WORK_POOL_H
