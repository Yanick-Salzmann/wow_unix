#include "light_manager.hpp"

#include "gl/mesh.h"
#include "glm/gtx/norm.hpp"
#include "utils/constants.h"

namespace wow::scene::sky {
    constexpr uint64_t DAY_LENGTH_MS = 86400000;
    constexpr uint64_t DAY_LENGTH_MIN = 1440;

    glm::vec4 light_manager::to_vec4(const uint32_t color) {
        return glm::vec4{
            (color & 0xFF) / 255.0f,
            ((color >> 8) & 0xFF) / 255.0f,
            ((color >> 16) & 0xFF) / 255.0f,
            ((color >> 24) & 0xFF) / 255.0f
        };
    }

    glm::vec4 light_manager::interpolate_color(const io::dbc::light_int_band_record &record, uint64_t time) {
        if (record.num_entries == 0) {
            return glm::vec4(0.0f);
        }

        auto last = to_vec4(record.colors[record.num_entries - 1]);
        auto last_time = record.times[record.num_entries - 1] - 1440;
        auto first = to_vec4(record.colors[0]);
        auto first_time = record.times[0] + 1440;

        for (auto i = 0; i < record.num_entries; ++i) {
            const auto cur_time = record.times[i];
            if (i == 0 && cur_time >= time) {
                const auto t = static_cast<float>(time - last_time) / static_cast<float>(cur_time - last_time);
                return glm::mix(last, to_vec4(record.colors[i]), t);
            }

            if ((i == record.num_entries - 1) && cur_time <= time) {
                const auto t = static_cast<float>(time - cur_time) / static_cast<float>(first_time - cur_time);
                return glm::mix(to_vec4(record.colors[i]), first, t);
            }

            const auto next_time = record.times[i + 1];
            if (time >= cur_time && time < next_time) {
                const auto t = static_cast<float>(time - cur_time) / static_cast<float>(next_time - cur_time);
                return glm::mix(to_vec4(record.colors[i]), to_vec4(record.colors[i + 1]), t);
            }
        }

        return to_vec4(record.colors[record.num_entries - 1]);
    }

    void light_manager::enter_world(const int32_t map_id) {
        _map_lights.clear();
        for (const auto &val: *_dbc_manager->light_dbc() | std::views::values) {
            if (val.map_id == map_id) {
                _map_lights.push_back(val);
            }
        }

        _current_map = map_id;
        _time_of_day_ms = 0;
    }

    void light_manager::on_update() {
        if (_current_map < 0 || _map_lights.empty()) {
            return;
        }

        const auto now = std::chrono::steady_clock::now();

        const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_update).count() * 1000;
        _last_update = now;

        _time_of_day_ms += diff_ms;
        _time_of_day_ms %= DAY_LENGTH_MS;

        const auto day_minutes = (_time_of_day_ms / 1000 / 60) % DAY_LENGTH_MIN;

        for (const auto &light: _map_lights) {
            const auto x = (light.x / 36.0f);
            const auto y = (light.z / 36.0f);
            const auto z = light.y / 36.0f;

            const auto is_global = light.x == 0 && light.y == 0 && light.z == 0;

            const auto max_distance = (light.falloff_end / 36.0f) * (light.falloff_end / 36.0f);
            if (const auto distance = glm::distance2(_position, glm::vec3(x, y, z));
                distance > max_distance && !is_global) {
                continue;
            }

            const auto fog_record = _dbc_manager->light_int_band_dbc()->record(light.params_clear * 18 - 17 + 7);
            _fog_color = interpolate_color(fog_record, day_minutes);
        }

        gl::mesh::terrain_mesh().apply_fog_color(_fog_color);
    }

    void light_manager::update_position(const glm::vec3 position) {
        _position = position;
        _position_changed = true;
    }
}
