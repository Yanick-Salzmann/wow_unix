#ifndef WOW_UNIX_IPC_MESSAGE_HANDLER_H
#define WOW_UNIX_IPC_MESSAGE_HANDLER_H

#undef Success
#undef Status

#include "js_event.pb.h"
#include "event/event_manager.h"
#include "include/wrapper/cef_message_router.h"

namespace wow::web {
    class ipc_message_handler : public CefMessageRouterBrowserSide::Handler {
        CefRefPtr<Callback> _callback{};

        event::event_manager_ptr _event_manager{};

    public:
        explicit ipc_message_handler(event::event_manager_ptr event_manager) : _event_manager(
            std::move(event_manager)) {
        }

        bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id,
                     const CefString &request, bool persistent, CefRefPtr<Callback> callback) override;
    };
}

#endif //WOW_UNIX_IPC_MESSAGE_HANDLER_H
