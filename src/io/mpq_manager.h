#ifndef WOW_UNIX_MPQ_MANAGER_H
#define WOW_UNIX_MPQ_MANAGER_H
#include <filesystem>
#include <functional>
#include <memory>

#include "web/event/event_manager.h"

namespace wow::io {
    class mpq_manager {
        std::vector<std::string> load_files(const std::filesystem::path& directory);
    public:
        mpq_manager(web::event::event_manager_ptr event_manager);

        void load_from_folder(const std::string &folder, const std::function<void(int, const std::string &)> &callback);
    };

    typedef std::shared_ptr<mpq_manager> mpq_manager_ptr;
}

#endif //WOW_UNIX_MPQ_MANAGER_H
