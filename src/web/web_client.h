#ifndef WOW_UNIX_WEB_CLIENT_H
#define WOW_UNIX_WEB_CLIENT_H

#include "gl/window.h"
#undef Success
#include "include/cef_client.h"
#undef Success
#include "ipc_message_handler.h"
#include "include/wrapper/cef_message_router.h"


namespace wow::web {
    class web_core;

    class web_client final :
            public CefClient,
            public CefRenderHandler,
            public CefLifeSpanHandler,
            public CefDisplayHandler,
            public CefRequestHandler,
            public CefFocusHandler {
        IMPLEMENT_REFCOUNTING(web_client);

        gl::window_ptr _window{};
        std::weak_ptr<web_core> _core{};

        CefRefPtr<CefBrowser> _browser{};

        CefRefPtr<CefMessageRouterBrowserSide> _router{};

    public:
        web_client(gl::window_ptr window, const std::shared_ptr<web_core>& core);

        CefRefPtr<CefRenderHandler> GetRenderHandler() override {
            return this;
        }

        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
            return this;
        }

        CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
            return this;
        }

        CefRefPtr<CefRequestHandler> GetRequestHandler() override {
            return this;
        }

        CefRefPtr<CefFocusHandler> GetFocusHandler() override {
            return this;
        }

        void OnGotFocus(CefRefPtr<CefBrowser> browser) override;

        bool OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source) override;

        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

        bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int &screenX, int &screenY) override;

        bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) override;

        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                     const void *buffer, int width, int height) override;

        void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                                const CefAcceleratedPaintInfo &info) override;

        void OnAfterCreated(const CefRefPtr<CefBrowser> browser) override;

        bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString &message,
                              const CefString &source, int line) override;

        bool OnCursorChange(CefRefPtr<CefBrowser> browser, cef_cursor_handle_t cursor, cef_cursor_type_t type,
                            const CefCursorInfo &custom_cursor_info) override;

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                      CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

        void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status, int error_code,
                                       const CefString &error_string) override;

        bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request,
                            bool user_gesture, bool is_redirect) override;

        void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;


        CefRefPtr<CefBrowser> browser() const {
            return _browser;
        }

        void notify_focus(bool focus) const;
    };
}

#endif //WOW_UNIX_WEB_CLIENT_H
