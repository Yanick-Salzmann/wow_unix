#include "dialog_utils.h"

#include <gtk/gtk.h>

#include "spdlog/spdlog.h"

namespace wow::utils {
    bool browse_folder_dialog(const std::string &title, std::string &path) {
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
    }
}
