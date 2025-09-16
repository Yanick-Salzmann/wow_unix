#ifndef WOW_UNIX_IPC_MESSAGE_HANDLER_H
#define WOW_UNIX_IPC_MESSAGE_HANDLER_H

#undef Success
#undef Status
#include "js_event.pb.h"
#include "include/wrapper/cef_message_router.h"

namespace wow::web {
    class ipc_message_handler : public CefMessageRouterBrowserSide::Handler {
        CefRefPtr<Callback> _callback{};
    public:
        bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id,
                     const CefString &request, bool persistent, CefRefPtr<Callback> callback) override;
    };
}

#endif //WOW_UNIX_IPC_MESSAGE_HANDLER_H