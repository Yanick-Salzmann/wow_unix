#include "light_manager.hpp"

#include "gl/mesh.h"

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

        gl::mesh::terrain_mesh().apply_fog_color(_fog_color);
    }
}
