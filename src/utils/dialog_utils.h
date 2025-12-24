#ifndef WOW_UNIX_DIALOG_UTILS_H
#define WOW_UNIX_DIALOG_UTILS_H

#include <string>

namespace wow::utils {
    bool browse_folder_dialog(const std::string &title, std::string &path);
}

#endif //WOW_UNIX_DIALOG_UTILS_H