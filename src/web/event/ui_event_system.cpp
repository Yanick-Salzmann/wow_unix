#include "ui_event_system.h"

#include "utils/string_utils.h"

namespace wow::web::event {
    proto::ListMapsResponse ui_event_system::handle_list_maps() const {
        proto::ListMapsResponse response{};

        for (const auto &[id, record]: *_dbc_manager->map_dbc()) {
            const auto entry = response.add_maps();
            entry->set_map_id(id);
            entry->set_name(record.name.text);
            if (_dbc_manager->loading_screen_dbc()->has_record(record.loading_screen)) {
                entry->set_loading_screen("blp://localhost/" +
                                          utils::replace_all(
                                              _dbc_manager->loading_screen_dbc()->record(record.loading_screen).path,
                                              "\\", "/"
                                          ));
            } else {
                entry->set_loading_screen("blp://localhost/Interface/Glues/loading.blp");
            }
        }

        return response;
    }

    proto::ListMapPoisResponse ui_event_system::handle_list_map_pois(int32_t map_id) const {
        proto::ListMapPoisResponse response{};
        response.set_map_id(map_id);

        for (const auto &val: *_dbc_manager->area_poi_dbc() | std::ranges::views::values) {
            if (val.map_id != map_id) {
                continue;
            }

            const auto poi = response.add_pois();
            poi->set_id(val.id);
            poi->set_name(val.name.text);
            poi->set_x(val.x);
            poi->set_y(val.y);
        }
        return response;
    }

    ui_event_system::ui_event_system(
        const event_manager_ptr &event_manager,
        const io::dbc::dbc_manager_ptr &dbc_manager,
        const scene::world_frame_ptr &world_frame
    ) : _dbc_manager(dbc_manager),
        _world_frame(world_frame) {
        event_manager->listen(proto::JsEvent::kListMapsRequest, [this](const auto &) {
            const auto lmr = handle_list_maps();
            auto response = proto::JsEvent{};
            response.mutable_list_maps_response()->CopyFrom(lmr);
            return response;
        });

        event_manager->listen(proto::JsEvent::kListMapPoisRequest, [this](const auto &req) {
            const auto rp = handle_list_map_pois(req.list_map_pois_request().map_id());
            auto response = proto::JsEvent{};
            response.mutable_list_map_pois_response()->CopyFrom(rp);
            return response;
        });

        event_manager->listen(proto::JsEvent::kEnterWorldRequest, [this, event_manager](const auto &req) {
            const glm::vec2 pos{
                req.enter_world_request().x(),
                req.enter_world_request().y()
            };

            _world_frame->enter_world(req.enter_world_request().map_id(), pos);
            return event_manager->empty_response();
        });
    }
}
