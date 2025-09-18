#ifndef WOW_UNIX_MPQ_MANAGER_H
#define WOW_UNIX_MPQ_MANAGER_H
#include <filesystem>
#include <functional>
#include <memory>
#include <regex>

#include "StormPort.h"
#include "web/event/event_manager.h"

namespace wow::io {
    class mpq_manager {
        static std::vector<std::string> load_files(const std::filesystem::path &directory);

        static std::string find_locale(const std::filesystem::path &directory);

        std::vector<HANDLE> _archives{};

        uint32_t _total_mpq_count = 0;
        uint32_t _loaded_mpq_count = 0;
        std::string _locale;

        static void load_base_mpq_files(const std::vector<std::string> &all_mpqs, std::vector<std::string> &out_files);

        void load_locale_mpq_files(const std::vector<std::string> &all_mpqs,
                                          std::vector<std::string> &out_files);

        static void load_mpqs_by_patterns(const std::vector<std::regex> &patterns,
                                          const std::vector<std::filesystem::path> &files,
                                          std::vector<std::string> &out_files);

    public:
        explicit mpq_manager(web::event::event_manager_ptr event_manager);

        void load_from_folder(const std::string &folder, const std::function<void(int, const std::string &)> &callback);
    };

    typedef std::shared_ptr<mpq_manager> mpq_manager_ptr;
}

#endif //WOW_UNIX_MPQ_MANAGER_H
