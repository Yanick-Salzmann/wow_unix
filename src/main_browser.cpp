#include <include/cef_app.h>

#include "include/wrapper/cef_message_router.h"
#include "spdlog/spdlog.h"

class app final : public CefApp, public CefRenderProcessHandler {
    IMPLEMENT_REFCOUNTING(app);

    CefRefPtr<CefMessageRouterRendererSide> _router = CefMessageRouterRendererSide::Create({});

public:
    void OnRegisterCustomSchemes(const CefRawPtr<CefSchemeRegistrar> registrar) override {
        for (const std::vector<std::string> schemes{"app", "blp", "minimap"};
                auto &scheme: schemes) {
            registrar->AddCustomScheme(
                scheme,
                CEF_SCHEME_OPTION_STANDARD
            );
                }
    }

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    void OnContextCreated(const CefRefPtr<CefBrowser> browser, const CefRefPtr<CefFrame> frame,
                          const CefRefPtr<CefV8Context> context) override {
        _router->OnContextCreated(browser, frame, context);
    }

    void OnContextReleased(const CefRefPtr<CefBrowser> browser, const CefRefPtr<CefFrame> frame,
                           const CefRefPtr<CefV8Context> context) override {
        _router->OnContextReleased(browser, frame, context);
    }

    bool OnProcessMessageReceived(const CefRefPtr<CefBrowser> browser, const CefRefPtr<CefFrame> frame, const CefProcessId source_process,
                                  const CefRefPtr<CefProcessMessage> message) override {
        return _router->OnProcessMessageReceived(browser, frame, source_process, message);
    }
};

int main(int argc, char *argv[]) {
    const CefMainArgs main_args(argc, argv);

    const CefRefPtr<CefApp> app = new ::app();

    return CefExecuteProcess(main_args, app, nullptr);
}
