#include "light_manager.hpp"

#include "gl/mesh.h"
#include "glm/gtx/norm.hpp"

namespace wow::scene::sky {
    void light_manager::enter_world(const int32_t map_id) {
        _map_lights.clear();
        for (const auto &val: *_dbc_manager->light_dbc() | std::views::values) {
            if (val.map_id == map_id) {
                _map_lights.push_back(val);
            }
        }

        _current_map = map_id;
    }

    void light_manager::on_update() {
        if (_current_map < 0 || _map_lights.empty()) {
            return;
        }

        if (!_position_changed) {
            return;
        }

        _position_changed = false;

        for (const auto &light: _map_lights) {
            const auto max_distance = light.falloff_end * light.falloff_end;
            if (const auto distance = glm::distance2(_position, glm::vec3(light.x, light.y, light.z));
                distance > max_distance) {
                continue;
            }

            const auto fog_record = _dbc_manager->light_int_band_dbc()->record(light.params_clear * 18 - 17 + 7);
            SPDLOG_INFO("Fog color: {}", fog_record.id);
        }

        gl::mesh::terrain_mesh().apply_fog_color(_fog_color);
    }

    void light_manager::update_position(const glm::vec3 position) {
        _position = position;
        _position_changed = true;
    }
}
