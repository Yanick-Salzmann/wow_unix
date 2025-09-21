#include <include/cef_app.h>

#include "include/wrapper/cef_message_router.h"
#include "spdlog/spdlog.h"

class app : public CefApp, public CefRenderProcessHandler {
    IMPLEMENT_REFCOUNTING(app);

    CefRefPtr<CefMessageRouterRendererSide> _router = CefMessageRouterRendererSide::Create({});

public:
    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
        for (const std::vector<std::string> schemes{"app", "blp", "minimap"};
                auto &scheme: schemes) {
            registrar->AddCustomScheme(
                scheme,
                CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED |
                CEF_SCHEME_OPTION_SECURE | CEF_SCHEME_OPTION_CSP_BYPASSING
            );
                }
    }

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override {
        _router->OnContextCreated(browser, frame, context);
    }

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) override {
        _router->OnContextReleased(browser, frame, context);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override {
        return _router->OnProcessMessageReceived(browser, frame, source_process, message);
    }
};

int main(int argc, char *argv[]) {
    CefMainArgs main_args(argc, argv);

    CefRefPtr<CefApp> app = new ::app();

    return CefExecuteProcess(main_args, app, nullptr);
}
