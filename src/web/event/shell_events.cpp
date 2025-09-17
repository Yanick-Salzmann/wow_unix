#include "shell_events.h"

#include "utils/dialog_utils.h"

namespace wow::web::event {
    proto::JsEvent browse_folder_request(const proto::BrowseFolderRequest &req) {
        proto::JsEvent event{};
        auto &resp = *event.mutable_browse_folder_response();
        std::string path{};
        if (!utils::browse_folder_dialog(req.title(), path)) {
            resp.set_canceled(true);
            return event;
        }

        resp.set_path(path);
        resp.set_canceled(false);
        return event;
    }

    void initialize_shell_events(const event_manager_ptr &event_manager) {
        event_manager->listen(proto::JsEvent::EventCase::kBrowseFolderRequest, [](const proto::JsEvent &event) {
            return browse_folder_request(event.browse_folder_request());
        });
    }
}
