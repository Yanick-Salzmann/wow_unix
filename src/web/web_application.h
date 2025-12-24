#ifndef WOW_UNIX_WEB_APPLICATION_H
#define WOW_UNIX_WEB_APPLICATION_H

#include <include/cef_app.h>

#include "spdlog/spdlog.h"

namespace wow::web {
    class web_application final : public CefApp, public CefBrowserProcessHandler {
        IMPLEMENT_REFCOUNTING(web_application);

    public:
        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
            return this;
        }

        void OnRegisterCustomSchemes(const CefRawPtr<CefSchemeRegistrar> registrar) override {
            for (const std::vector<std::string> schemes{"app", "blp", "minimap"};
                 auto &scheme: schemes) {
                registrar->AddCustomScheme(
                    scheme,
                    CEF_SCHEME_OPTION_STANDARD
                );
            }
        }
    };
}

#endif //WOW_UNIX_WEB_APPLICATION_H
