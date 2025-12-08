#include "light_manager.hpp"

#include "gl/mesh.h"
#include "glm/gtx/norm.hpp"

namespace wow::scene::sky {
    constexpr uint64_t DAY_LENGTH_MS = 86400000;
    constexpr uint64_t DAY_LENGTH_MIN = 1440;

    glm::vec4 light_manager::to_vec4(const uint32_t color) {
        return glm::vec4{
            static_cast<float>(color & 0xFF) / 255.0f,
            static_cast<float>((color >> 8) & 0xFF) / 255.0f,
            static_cast<float>((color >> 16) & 0xFF) / 255.0f,
            static_cast<float>((color >> 24) & 0xFF) / 255.0f
        };
    }

    glm::vec4 light_manager::interpolate_color(const io::dbc::light_int_band_record &record, const uint64_t time) {
        if (record.num_entries == 0) {
            return glm::vec4(0.0f);
        }

        const auto last = to_vec4(record.colors[record.num_entries - 1]);
        const auto last_time = record.times[record.num_entries - 1] - 1440;
        const auto first = to_vec4(record.colors[0]);
        const auto first_time = record.times[0] + 1440;

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

            if (const auto next_time = record.times[i + 1];
                time >= cur_time && time < next_time) {
                const auto t = static_cast<float>(time - cur_time) / static_cast<float>(next_time - cur_time);
                return glm::mix(to_vec4(record.colors[i]), to_vec4(record.colors[i + 1]), t);
            }
        }

        return to_vec4(record.colors[record.num_entries - 1]);
    }

    void light_manager::update_weights() {
        for (auto i = static_cast<ssize_t>(_map_lights.size()) - 1; i >= 0; --i) {
            const auto &light = _map_lights[i];
            if (light.x == 0 && light.y == 0 && light.z == 0) {
                continue;
            }

            const auto px = light.x / 36.0f;
            const auto py = light.z / 36.0f;
            const auto pz = light.y / 36.0f;

            const auto distance = glm::distance2(_position, glm::vec3(px, py, pz));
            const auto falloff_end = (light.falloff_end / 36.0f) * (light.falloff_end / 36.0f);
            const auto falloff_start = (light.falloff_start / 36.0f) * (light.falloff_start / 36.0f);

            if (distance > falloff_end) {
                _weights[i] = 0.0f;
            } else if (distance > falloff_start) {
                const auto weight = (distance - falloff_start) / (falloff_end - falloff_start);
                _weights[i] = 1.0f - weight;
                for (auto j = i + 1; j < _map_lights.size(); ++j) {
                    _weights[j] *= weight;
                }
            } else {
                _weights[i] = 1.0f;
                for (auto j = i + 1; j < _map_lights.size(); ++j) {
                    _weights[j] = 0.0f;
                }
            }
        }

        const auto total_weight = std::accumulate(_weights.begin(), _weights.end(), 0.0f);
        if (total_weight >= 1.0f || _global_lights.empty()) {
            return;
        }

        for (const auto _global_light: _global_lights) {
            _weights[_global_light] = (1.0f - total_weight) / static_cast<float>(_global_lights.size());
        }
    }

    void light_manager::enter_world(const int32_t map_id) {
        _map_lights.clear();

        for (const auto &val: *_dbc_manager->light_dbc() | std::views::values) {
            if (val.map_id == map_id) {
                _map_lights.push_back(val);
            }
        }

        _weights.resize(_map_lights.size());
        std::ranges::sort(_map_lights, [](const auto &a, const auto &b) {
            const auto a_global = a.x == 0 && a.y == 0 && a.z == 0;
            const auto b_global = b.x == 0 && b.y == 0 && b.z == 0;

            if (a_global && !b_global) {
                return true;
            }

            if (!a_global && b_global) {
                return false;
            }

            return a.falloff_end < b.falloff_end;
        });

        for (auto i = 0; i < _map_lights.size(); ++i) {
            if (const auto &light = _map_lights[i];
                light.x == 0 && light.y == 0 && light.z == 0) {
                _global_lights.push_back(i);
            }
        }

        _current_map = map_id;
        _time_of_day_ms = 0;
    }

    void light_manager::on_update() {
        if (_current_map < 0 || _map_lights.empty()) {
            return;
        }

        if (_position_changed) {
            std::ranges::fill(_weights, 0.0f);
            update_weights();
            _position_changed = false;
        }

        const auto now = std::chrono::steady_clock::now();

        const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_update).count() * 4000;
        _last_update = now;

        _time_of_day_ms += diff_ms;
        _time_of_day_ms %= DAY_LENGTH_MS;

        const auto day_minutes = (_time_of_day_ms / 1000 / 60) % DAY_LENGTH_MIN;
        auto fog_color = glm::vec4(0.0f);
        for (auto i = 0; i < _map_lights.size(); ++i) {
            const auto &light = _map_lights[i];
            fog_color += interpolate_color(_dbc_manager->light_int_band_dbc()->record(light.params_clear * 18 - 17 + 7), day_minutes) * _weights[i];
        }

        _fog_color = fog_color;
        gl::mesh::terrain_mesh().apply_fog_color(_fog_color);
    }

    void light_manager::update_position(const glm::vec3 position) {
        _position = position;
        _position_changed = true;
    }
}
