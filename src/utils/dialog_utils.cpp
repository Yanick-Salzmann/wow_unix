#include "dialog_utils.h"

#ifndef _WIN32
#include <gtk/gtk.h>
#else
#include <shobjidl.h>
#include <shlobj_core.h>
#include <atlbase.h>
#include "utils/di.h"
#endif

#include "spdlog/spdlog.h"

namespace wow::utils {
    bool browse_folder_dialog(const std::string &title, std::string &path) {
#ifndef _WIN32
        constexpr auto action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

        const auto chooser = gtk_file_chooser_dialog_new(
            title.c_str(),
            nullptr,
            action,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            NULL
        );

        if (const gint dialog_result = gtk_dialog_run(GTK_DIALOG(chooser));
            dialog_result == GTK_RESPONSE_ACCEPT) {
            if (char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser))) {
                path = filename;
                g_free(filename);
            }
        }

        gtk_widget_destroy(GTK_WIDGET(chooser));
        return !path.empty();
#else
        CComPtr<IFileOpenDialog> open_dialog{};
        if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
            IID_PPV_ARGS(&open_dialog)))) {
            SPDLOG_ERROR("Failed to create IFileOpenDialog instance");
            return false;
        }

        std::wstring title_wide{};
        title_wide.resize(title.size());
        std::mbstowcs(title_wide.data(), title.c_str(), title.size());

        open_dialog->SetTitle(title_wide.c_str());

        FILEOPENDIALOGOPTIONS opts{};
        opts |= FOS_PICKFOLDERS;
        open_dialog->SetOptions(opts);

        if (SUCCEEDED(open_dialog->Show(app_module->window()->handle()))) {
            CComPtr<IShellItem> result_item{};
            if (SUCCEEDED(open_dialog->GetResult(&result_item))) {
                PWSTR folder_path{};
                if (SUCCEEDED(result_item->GetDisplayName(SIGDN_FILESYSPATH, &folder_path))) {
                    std::wstring folder_path_wide{folder_path};
                    path.resize(folder_path_wide.size());
                    std::wcstombs(path.data(), folder_path_wide.c_str(), folder_path_wide.size());
                    CoTaskMemFree(folder_path);
                    return true;
                }
            }
        }
        return "";
#endif
    }
}
