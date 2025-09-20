#include "web_client.h"
#include "web_core.h"

#include <utility>

#include "ipc_message_handler.h"
#include "spdlog/spdlog.h"

namespace wow::web {
    web_client::web_client(gl::window_ptr window, const std::shared_ptr<web_core> &core) : _window(std::move(window)),
        _core(core) {
        _router = CefMessageRouterBrowserSide::Create(CefMessageRouterConfig());
        _router->AddHandler(new ipc_message_handler(core->event_manager()), true);
    }

    void web_client::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
        if (!_window) {
            return;
        }

        auto [w, h] = _window->size();
        auto scale = _window->dpi_scaling();

        rect.x = 0;
        rect.y = 0;
        rect.width = static_cast<int>(w / scale);
        rect.height = static_cast<int>(h / scale);
    }

    bool web_client::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int &screenX, int &screenY) {
        auto [x, y] = _window->screen_coordinates(viewX, viewY);
        screenX = x;
        screenY = y;
        return true;
    }

    bool web_client::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) {
        auto monitor = glfwGetWindowMonitor(_window->native_handle());
        if (!monitor) {
            monitor = glfwGetPrimaryMonitor();
        }

        glfwGetMonitorWorkarea(monitor, &screen_info.rect.x, &screen_info.rect.y, &screen_info.rect.width,
                               &screen_info.rect.height);

        screen_info.device_scale_factor = _window->dpi_scaling();
        screen_info.available_rect = screen_info.rect;
        return true;
    }

    void web_client::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                             const void *buffer, int width, int height) {
        _core.lock()->on_paint(width, height, buffer);
    }

    void web_client::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                                        const RectList &dirtyRects, const CefAcceleratedPaintInfo &info) {
        SPDLOG_INFO("OnAcceleratedPaint");
    }

    bool web_client::OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString &message,
                                      const CefString &source, int line) {
        std::string sourceStr = source.ToString();
        if (sourceStr.starts_with("app://localhost/")) {
            sourceStr.replace(0, 16, "./ui/");
            sourceStr = absolute(std::filesystem::path{sourceStr}).string();
        }

        SPDLOG_LOGGER_CALL(spdlog::default_logger(), static_cast<spdlog::level::level_enum>(level),
                           "Web: {}:{}: {}", sourceStr, line, message.ToString());
        return true;
    }

    bool web_client::OnCursorChange(CefRefPtr<CefBrowser> browser, cef_cursor_handle_t cursor, cef_cursor_type_t type,
                                    const CefCursorInfo &custom_cursor_info) {
        _window->change_cursor(type);
        return true;
    }

    bool web_client::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                              CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
        return _router->OnProcessMessageReceived(browser, frame, source_process, message);
    }

    void web_client::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status, int error_code,
                                               const CefString &error_string) {
        _router->OnRenderProcessTerminated(browser);
    }

    bool web_client::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) {
        _router->OnBeforeBrowse(browser, frame);
        return false;
    }

    void web_client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
        _router->OnBeforeClose(browser);
    }
}
