#include "web_dialog_handler.h"
#include "spdlog/spdlog.h"

#ifndef _WIN32
#include <gtk/gtk.h>
#else
#include <shobjidl.h>
#include <shlobj_core.h>
#include <atlbase.h>
#include "utils/di.h"
#endif
#include <thread>
#include <vector>
#include <sstream>

#include "lambda_task.h"
#include "include/cef_task.h"
#include "include/base/cef_bind.h"

namespace wow::web {
    bool web_dialog_handler::OnFileDialog(CefRefPtr<CefBrowser> browser, const FileDialogMode mode,
                                          const CefString &title,
                                          const CefString &default_file_path,
                                          const std::vector<CefString> &accept_filters,
                                          const std::vector<CefString> &accept_extensions,
                                          const std::vector<CefString> &accept_descriptions,
                                          const CefRefPtr<CefFileDialogCallback> callback) {
        SPDLOG_DEBUG("OnFileDialog called with mode: {}", static_cast<int>(mode));
#ifndef _WIN32
        std::thread dialog_thread([=]() {
            if (!gtk_init_check(nullptr, nullptr)) {
                SPDLOG_ERROR("Failed to initialize GTK"); // NOLINT(*-lambda-function-name)
                callback->Cancel();
                return;
            }

            if (getenv("WAYLAND_DISPLAY")) {
                gdk_set_allowed_backends("wayland");
            }

            GtkFileChooserAction action;
            const char *confirm_button;

            if (mode == FILE_DIALOG_OPEN || mode == FILE_DIALOG_OPEN_MULTIPLE) {
                action = GTK_FILE_CHOOSER_ACTION_OPEN;
                confirm_button = "_Open";
            } else if (mode == FILE_DIALOG_SAVE) {
                action = GTK_FILE_CHOOSER_ACTION_SAVE;
                confirm_button = "_Save";
            } else if (mode == FILE_DIALOG_OPEN_FOLDER) {
                action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
                confirm_button = "_Select";
            } else {
                SPDLOG_ERROR("Unknown file dialog mode"); // NOLINT(*-lambda-function-name)
                callback->Cancel();
                return;
            }

            const auto chooser = GTK_FILE_CHOOSER(gtk_file_chooser_native_new(
                title.ToString().c_str(),
                nullptr,
                action,
                confirm_button,
                "_Cancel"
            ));

            if (mode == FILE_DIALOG_OPEN_MULTIPLE) {
                gtk_file_chooser_set_select_multiple(chooser, TRUE);
            }

            const std::string default_path = default_file_path.ToString();
            if (!default_path.empty()) {
                if (mode == FILE_DIALOG_SAVE) {
                    if (const size_t last_separator = default_path.find_last_of("/\\");
                        last_separator != std::string::npos && last_separator < default_path.length() - 1) {
                        gtk_file_chooser_set_current_folder(chooser,
                                                            default_path.substr(0, last_separator).c_str());
                        gtk_file_chooser_set_current_name(chooser,
                                                          default_path.substr(last_separator + 1).c_str());
                    } else {
                        gtk_file_chooser_set_current_name(chooser, default_path.c_str());
                    }
                } else {
                    gtk_file_chooser_set_current_folder(chooser, default_path.c_str());
                }
            }

            for (size_t i = 0; i < accept_filters.size(); ++i) {
                GtkFileFilter *filter = gtk_file_filter_new();
                std::string filter_str = accept_filters[i].ToString();

                if (i < accept_descriptions.size() && !accept_descriptions[i].empty()) {
                    gtk_file_filter_set_name(filter, accept_descriptions[i].ToString().c_str());
                } else {
                    gtk_file_filter_set_name(filter, filter_str.c_str());
                }

                std::string pattern;
                std::istringstream pattern_stream(filter_str);
                while (std::getline(pattern_stream, pattern, ';')) {
                    if (!pattern.empty()) {
                        gtk_file_filter_add_pattern(filter, pattern.c_str());
                    }
                }

                gtk_file_chooser_add_filter(chooser, filter);
            }

            for (const auto &ext: accept_extensions) {
                std::string mime_type = ext.ToString();
                if (!mime_type.empty()) {
                    GtkFileFilter *filter = gtk_file_filter_new();
                    gtk_file_filter_set_name(filter, mime_type.c_str());
                    gtk_file_filter_add_mime_type(filter, mime_type.c_str());
                    gtk_file_chooser_add_filter(chooser, filter);
                }
            }

            if (const gint result = gtk_native_dialog_run(GTK_NATIVE_DIALOG(chooser));
                result == GTK_RESPONSE_ACCEPT) {
                std::vector<CefString> selected_files;

                if (mode == FILE_DIALOG_OPEN_MULTIPLE) {
                    GSList *filenames = gtk_file_chooser_get_filenames(chooser);
                    for (const GSList *iter = filenames; iter; iter = iter->next) {
                        const auto filename = static_cast<char *>(iter->data);
                        selected_files.emplace_back(filename);
                        g_free(filename);
                    }
                    g_slist_free(filenames);
                } else {
                    if (char *filename = gtk_file_chooser_get_filename(chooser)) {
                        selected_files.emplace_back(filename);
                        g_free(filename);
                    }
                }

                CefPostTask(TID_UI, make_task([callback, selected_files] {
                    callback->Continue(selected_files);
                }));
            } else {
                CefPostTask(TID_UI, make_task([callback] { callback->Cancel(); }));
            }

            g_object_unref(chooser);
        });

        dialog_thread.detach();
        return true;
#else
        CComPtr<IFileOpenDialog> open_dialog{};
        if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
            IID_PPV_ARGS(&open_dialog)))) {
            SPDLOG_ERROR("Failed to create IFileOpenDialog instance");
            return false;
        }

        open_dialog->SetTitle(title.ToWString().c_str());
        LPITEMIDLIST pidl{};
        SHParseDisplayName(default_file_path.ToWString().c_str(), nullptr, &pidl, 0, nullptr);

        open_dialog->Show(utils::app_module->window()->handle());
        return false;
#endif
    }
}
