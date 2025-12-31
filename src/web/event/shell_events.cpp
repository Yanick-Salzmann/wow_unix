#include "shell_events.h"

#include "utils/dialog_utils.h"

namespace wow::web::event {
    js_event browse_folder_request(const browse_folder_request &req) {
        js_event event{};
        event.type = js_event_type::browse_folder_response;
        auto &[cancelled, path] = event.browse_folder_response_data;
        std::string target_path{};
        if (!utils::browse_folder_dialog(req.title, target_path)) {
            cancelled = true;
            return event;
        }

        path = target_path;
        cancelled = false;
        return event;
    }

    void initialize_shell_events(const event_manager_ptr &event_manager) {
        event_manager->listen(js_event_type::browse_folder_request, [](const js_event &event) {
            return browse_folder_request(event.browse_folder_request_data);
        });
    }
}
