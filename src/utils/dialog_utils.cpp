#include "dialog_utils.h"

#include <gtk/gtk.h>

#include "spdlog/spdlog.h"

namespace wow::utils {
    bool browse_folder_dialog(const std::string &title, std::string &path) {
        if (!gtk_init_check(nullptr, nullptr)) {
            spdlog::error("Failed to initialize GTK");
            return false;
        }

        if (getenv("WAYLAND_DISPLAY")) {
            gdk_set_allowed_backends("wayland");
        }

        const auto chooser = GTK_FILE_CHOOSER(gtk_file_chooser_native_new(
            title.c_str(),
            nullptr,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            "_Select",
            "_Cancel"
        ));

        if (const gint result = gtk_native_dialog_run(GTK_NATIVE_DIALOG(chooser));
            result == GTK_RESPONSE_ACCEPT) {
            if (char *filename = gtk_file_chooser_get_filename(chooser)) {
                path = filename;
                g_free(filename);
            }
        }

        g_object_unref(chooser);
        return !path.empty();
    }
}
