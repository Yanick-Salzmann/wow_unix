#include "ui_event_system.h"

namespace wow::web::event {
    proto::ListMapsResponse ui_event_system::handle_list_maps() const {
        proto::ListMapsResponse response{};

        for (const auto &[id, record]: *_dbc_manager->map_dbc()) {
            const auto entry = response.add_maps();
            entry->set_map_id(id);
            entry->set_name(record.name.text);
            if (_dbc_manager->loading_screen_dbc()->has_record(record.loading_screen)) {
                entry->set_loading_screen(_dbc_manager->loading_screen_dbc()->record(record.loading_screen).name);
            }
        }

        return response;
    }

    ui_event_system::ui_event_system(const event_manager_ptr &event_manager,
                                     const io::dbc::dbc_manager_ptr &dbc_manager)
        : _dbc_manager(dbc_manager) {
        event_manager->listen(proto::JsEvent::kListMapsRequest, [this](const auto &) {
            const auto lmr = handle_list_maps();
            auto response = proto::JsEvent{};
            response.mutable_list_maps_response()->CopyFrom(lmr);
            return response;
        });
    }
}
