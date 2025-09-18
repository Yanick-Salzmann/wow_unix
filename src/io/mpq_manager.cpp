#include "mpq_manager.h"

#include <filesystem>

#include "spdlog/spdlog.h"
#include "utils/string_utils.h"

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

            SPDLOG_INFO("Loading mpq: {}", absolute(itr->path()).string());
            files.push_back(absolute(itr->path()).string());
        }

        return files;
    }

    mpq_manager::mpq_manager(web::event::event_manager_ptr event_manager) {
        event_manager->listen(web::proto::JsEvent::kLoadDataEvent,
                              [this, event_manager](const web::proto::JsEvent &event) {
                                  load_from_folder(event.load_data_event().folder(),
                                                   [event_manager](int progress, const std::string &msg) {
                                                       auto ev = web::proto::JsEvent{};
                                                       const auto update = ev.mutable_load_update_event();
                                                       update->set_message(msg);
                                                       update->set_percentage(progress);
                                                       update->set_completed(progress >= 100);

                                                       event_manager->submit(ev);
                                                   });
                                  return event_manager->empty_response();
                              });
    }

    void mpq_manager::load_from_folder(const std::string &folder,
                                       const std::function<void(int, const std::string &)> &callback) {
        std::filesystem::path data_path{folder};
        data_path /= "Data";
        auto root_mpq_files = load_files(data_path);
        callback(0, "Loading MPQ files...");
    }
}
