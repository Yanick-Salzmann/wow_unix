#include "ui_event_system.h"

#include <ranges>
#include <utility>

#include "utils/string_utils.h"

namespace wow::web::event {
    list_maps_response ui_event_system::handle_list_maps() const {
        list_maps_response response{};

        for (const auto &[id, record]: *_dbc_manager->map_dbc()) {
            auto entry = list_maps_response_map{};
            entry.map_id = id;
            entry.name = fmt::format("{} ({})", record.name.text, record.directory);
            if (_dbc_manager->loading_screen_dbc()->has_record(record.loading_screen)) {
                entry.loading_screen = "blp://localhost/" +
                                       utils::replace_all(
                                           _dbc_manager->loading_screen_dbc()->record(record.loading_screen).path,
                                           "\\", "/"
                                       );
            } else {
                entry.loading_screen = "blp://localhost/Interface/Glues/loading.blp";
            }

            response.maps.push_back(std::move(entry));
        }

        return response;
    }

    list_map_pois_response ui_event_system::handle_list_map_pois(const int32_t map_id) const {
        list_map_pois_response response{};
        response.map_id = map_id;

        for (const auto &val: *_dbc_manager->area_poi_dbc() | std::ranges::views::values) {
            if (val.map_id != map_id) {
                continue;
            }

            auto poi = map_poi{};
            poi.id = val.id;
            poi.name = val.name.text;
            poi.x = val.x;
            poi.y = val.y;

            response.pois.push_back(std::move(poi));
        }

        return response;
    }

    ui_event_system::ui_event_system(
        const event_manager_ptr &event_manager,
        io::dbc::dbc_manager_ptr dbc_manager,
        scene::world_frame_ptr world_frame
    ) : _dbc_manager(std::move(dbc_manager)),
        _world_frame(std::move(world_frame)),
        _event_manager(event_manager) {
        event_manager->listen(js_event_type::list_maps_request, [this](const js_event &) {
            const auto lmr = handle_list_maps();
            auto response = js_event{};
            response.type = js_event_type::list_maps_response;
            response.list_maps_response_data = lmr;
            return response;
        });

        event_manager->listen(js_event_type::list_map_pois_request, [this](const js_event &req) {
            const auto rp = handle_list_map_pois(req.list_map_pois_request_data.map_id);
            auto response = js_event{};
            response.type = js_event_type::list_map_pois_response;
            response.list_map_pois_response_data = rp;
            return response;
        });

        event_manager->listen(js_event_type::enter_world_request, [this, event_manager](const js_event &req) {
            const glm::vec2 pos{
                req.enter_world_request_data.x,
                req.enter_world_request_data.y
            };

            _world_frame->enter_world(req.enter_world_request_data.map_id, pos);
            return event_manager->empty_response();
        });

        event_manager->listen(js_event_type::fetch_game_time_request, [this](const js_event &) {
            const auto time = _world_frame->map_manager()->light_manager()->time_of_day();

            auto response = js_event{};
            response.type = js_event_type::fetch_game_time_response;
            response.fetch_game_time_response_data.time_of_day = time;
            return response;
        });
    }
}
