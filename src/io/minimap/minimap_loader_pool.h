#ifndef WOW_UNIX_MINIMAP_LOADER_POOL_H
#define WOW_UNIX_MINIMAP_LOADER_POOL_H
#include <future>
#include <list>
#include <queue>
#include <thread>

namespace wow::io::minimap {
    class minimap_loader_pool {
        typedef std::shared_ptr<std::packaged_task<void()>> work_item_ptr;

        std::vector<std::thread> _worker_threads{};
        std::mutex _work_lock{};
        std::condition_variable _work_cv{};

        std::queue<work_item_ptr> _work_queue{};

        bool _running = true;

        void worker_function();

    public:
        minimap_loader_pool();
        ~minimap_loader_pool();

        std::shared_future<void> submit(const std::function<void()>& task);
    };
}

#endif //WOW_UNIX_MINIMAP_LOADER_POOL_H