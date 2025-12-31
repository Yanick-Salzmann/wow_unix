#include "mpq_manager.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "StormLib.h"
#include "blp/blp_file.h"
#include "spdlog/spdlog.h"
#include "utils/string_utils.h"

#include "dbc/dbc_manager.h"

namespace fs = std::filesystem;

namespace wow::io {
    std::vector<std::string> mpq_manager::load_files(const std::filesystem::path &directory) {
        std::vector<std::string> files{};
        for (auto itr = fs::directory_iterator{directory}; itr != fs::directory_iterator{}; ++itr) {
            if (!itr->is_regular_file()) {
                continue;
            }

            if (utils::to_lower(itr->path().extension().string()) != ".mpq") {
                continue;
            }

            files.push_back(absolute(itr->path()).string());
        }

        return files;
    }

    std::string mpq_manager::find_locale(const std::filesystem::path &directory) {
        static std::vector<std::string> locales{"enUS", "enGB", "deDE", "frFR", "esES", "esMX", "ruRU", "zhCN", "zhTW"};
        for (const auto &locale: locales) {
            if (fs::exists(directory / locale)) {
                return locale;
            }
        }

        return "";
    }

    void mpq_manager::load_base_mpq_files(const std::vector<std::string> &all_mpqs,
                                          std::vector<std::string> &out_files) {
        static std::regex_constants::syntax_option_type syntax{
            std::regex_constants::icase | std::regex_constants::ECMAScript
        };

        static std::regex COMMON{"COMMON(-[0-9a-zA-Z]+)?\\.MPQ$", syntax};
        static std::regex PATCH{"PATCH\\.MPQ$", syntax};
        static std::regex CUSTOM_PATCH{"PATCH-[0-9a-zA-Z]+\\.MPQ$", syntax};
        static std::regex EXPANSION{"EXPANSION\\.MPQ$", syntax};
        static std::regex LICHKING{"LICHKING\\.MPQ$", syntax};

        std::vector<std::filesystem::path> files{};
        for (auto &mpq: all_mpqs) {
            files.emplace_back(mpq);
        }

        load_mpqs_by_patterns({COMMON, PATCH, EXPANSION, LICHKING, CUSTOM_PATCH}, files, out_files);
    }

    void mpq_manager::load_locale_mpq_files(const std::vector<std::string> &all_mpqs,
                                            std::vector<std::string> &out_files) {
        static std::string locale_patterns[]{
            "backup-{}\\.MPQ$",
            "base-{}\\.MPQ$",
            "locale-{}\\.MPQ$",
            "patch-{}\\.MPQ$",
            "expansion-locale-{}\\.MPQ$",
            "lichking-locale-{}\\.MPQ$",
            "patch-{}-[0-9a-zA-Z]+\\.MPQ$",
            "speech-{}\\.MPQ$",
            "expansion-speech-{}\\.MPQ$",
            "lichking-speech-{}\\.MPQ$"
        };

        std::vector<std::regex> regex_values{};
        std::ranges::transform(locale_patterns, std::back_inserter(regex_values),
                               [this](const auto &pattern) {
                                   return std::regex{
                                       utils::replace_all(pattern, "{}", _locale),
                                       std::regex_constants::icase | std::regex_constants::ECMAScript
                                   };
                               });

        std::vector<std::filesystem::path> files{};
        for (auto &mpq: all_mpqs) {
            files.emplace_back(mpq);
        }

        load_mpqs_by_patterns(regex_values, files, out_files);
    }

    void mpq_manager::load_mpqs_by_patterns(const std::vector<std::regex> &patterns,
                                            const std::vector<std::filesystem::path> &files,
                                            std::vector<std::string> &out_files) {
        for (const auto &pattern: patterns) {
            std::vector<std::filesystem::path> matching_files{};

            for (const auto &file: files) {
                if (const auto extension = utils::to_lower(file.extension().string()); extension != ".mpq") {
                    continue;
                }

                if (std::regex_match(file.filename().string(), pattern)) {
                    matching_files.emplace_back(file);
                }
            }

            std::ranges::sort(matching_files, [](const auto &a, const auto &b) {
                return a.stem().string() < b.stem().string();
            });

            for (const auto &f: matching_files) {
                out_files.emplace_back(absolute(f).string());
            }
        }
    }

    mpq_manager::mpq_manager(web::event::event_manager_ptr event_manager,
                             const dbc::dbc_manager_ptr &dbc_manager) : _dbc_manager(dbc_manager) {
        event_manager->listen(web::event::js_event_type::load_data_event,
                              [this, event_manager](const web::event::js_event &event) {
                                  load_from_folder(event.load_data_event_data.folder,
                                                   [event_manager](const int progress, const std::string &msg) {
                                                       auto ev = web::event::js_event{};
                                                       ev.type = web::event::js_event_type::load_update_event;
                                                       ev.load_update_event_data.message = msg;
                                                       ev.load_update_event_data.percentage = progress;
                                                       ev.load_update_event_data.completed = progress >= 100;

                                                       event_manager->submit(ev);
                                                   });
                                  return event_manager->empty_response();
                              });
    }

    void mpq_manager::load_from_folder(const std::string &folder,
                                       const std::function<void(int, const std::string &)> &callback) {
        callback(0, "Searching MPQ files...");
        std::filesystem::path data_path{folder};
        data_path /= "Data";
        const auto root_mpq_files = load_files(data_path);
        std::vector<std::string> locale_mpq_files{};

        if (const auto locale = find_locale(data_path); !locale.empty()) {
            callback(5, "Searching locale MPQ files...");
            const auto locale_path = data_path / locale;
            locale_mpq_files = load_files(locale_path);
            _locale = locale;
        }

        std::vector<std::string> actual_base_mpq_files{}, actual_locale_mpq_files{};
        load_base_mpq_files(root_mpq_files, actual_base_mpq_files);
        load_locale_mpq_files(locale_mpq_files, actual_locale_mpq_files);
        actual_base_mpq_files.insert(actual_base_mpq_files.end(), actual_locale_mpq_files.begin(),
                                     actual_locale_mpq_files.end());

        auto total_mpq = actual_base_mpq_files.size();

        callback(10, fmt::format("Loading {} MPQ files...", total_mpq));

        auto processed = 0;
        for (auto &mpq: actual_base_mpq_files) {
            callback(10 + processed * 85 / total_mpq,
                     fmt::format("Loading {}...", std::filesystem::relative(mpq, data_path).string()));
            HANDLE handle{};
            if (!SFileOpenArchive(mpq.c_str(), 0, MPQ_OPEN_READ_ONLY, &handle)) {
                SPDLOG_WARN("Skipping MPQ file: {} due to loading error", mpq);
            } else {
                _archives.emplace(_archives.begin(), handle, mpq);
                SPDLOG_DEBUG("Loaded MPQ file: {}", mpq);
            }

            ++processed;
        }

        callback(95, "Loading DBC files...");
        _dbc_manager->initialize(shared_from_this(), callback);
        callback(100, "Done!");
    }

    mpq_file_ptr mpq_manager::open(const std::string &path) {
        std::lock_guard lock(_archive_lock);
        for (const auto &[handle, name]: _archives) {
            HANDLE file_handle{};
            if (SFileOpenFileEx(handle, path.c_str(), 0, &file_handle)) {
                SPDLOG_INFO("Opening file: {} in MPQ: {}", path, name);
                return std::make_shared<mpq_file>(file_handle);
            }
        }

        return nullptr;
    }
}
