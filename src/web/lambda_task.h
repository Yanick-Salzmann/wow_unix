#ifndef WOW_UNIX_LAMBDA_TASK_H
#define WOW_UNIX_LAMBDA_TASK_H
#include <functional>

#include "include/cef_task.h"

namespace wow::web {
    class lambda_task : public CefTask {
        IMPLEMENT_REFCOUNTING(lambda_task);
        std::function<void()> _task;

    public:
        explicit lambda_task(const std::function<void()> &task) : _task(task) {
        }

        void Execute() override {
            _task();
        }
    };

    inline CefRefPtr<CefTask> make_task(std::function<void()> task) {
        return new lambda_task(std::move(task));
    }
}

#endif //WOW_UNIX_LAMBDA_TASK_H
