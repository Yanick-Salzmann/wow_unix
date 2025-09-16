#ifndef WOW_UNIX_WEB_DIALOG_HANDLER_H
#define WOW_UNIX_WEB_DIALOG_HANDLER_H

#include "include/cef_dialog_handler.h"

namespace wow::web {
    class web_dialog_handler : public CefDialogHandler {
        IMPLEMENT_REFCOUNTING(web_dialog_handler);

    public:
        bool OnFileDialog(CefRefPtr<CefBrowser> browser, FileDialogMode mode, const CefString &title, const CefString &default_file_path, const std::vector<CefString> &accept_filters, const std::vector<CefString> &accept_extensions, const std::vector<CefString> &accept_descriptions, CefRefPtr<CefFileDialogCallback> callback) override;
    };
}

#endif //WOW_UNIX_WEB_DIALOG_HANDLER_H