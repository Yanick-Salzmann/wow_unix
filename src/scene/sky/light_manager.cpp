#include "light_manager.hpp"

#include "gl/mesh.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/norm.hpp"
#include "utils/constants.h"

namespace wow::scene::sky {
    constexpr uint64_t DAY_LENGTH_MS = 86400000;
    constexpr uint64_t DAY_LENGTH_HALF_MIN = 2880;

    constexpr float SUN_ALPHA = glm::quarter_pi<float>();

    glm::vec3 light_manager::calculate_sun_direction(uint32_t day_half_minutes) {
        const auto fraction = static_cast<float>(day_half_minutes) / static_cast<float>(DAY_LENGTH_HALF_MIN);
        const auto angle = fraction * glm::two_pi<float>() - glm::half_pi<float>();
        return {glm::cos(angle) * glm::cos(SUN_ALPHA), glm::sin(SUN_ALPHA), glm::sin(angle) * glm::cos(SUN_ALPHA)};
    }

    void light_manager::update_weights() {
        for (auto i = static_cast<ssize_t>(_map_lights.size()) - 1; i >= 0; --i) {
            const auto &light = _map_lights[i];
            if (light.is_global()) {
                continue;
            }

            const auto distance = glm::distance2(_position, light.position());
            const auto falloff_end = light.falloff_end() * light.falloff_end();
            const auto falloff_start = light.falloff_start() * light.falloff_start();

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
                _map_lights.push_back(light_data(_dbc_manager, val));
            }
        }

        _weights.resize(_map_lights.size());
        std::ranges::sort(_map_lights, [](const auto &a, const auto &b) {
            const auto a_global = a.is_global();
            const auto b_global = b.is_global();

            if (a_global && !b_global) {
                return true;
            }

            if (!a_global && b_global) {
                return false;
            }

            return a.falloff_end() < b.falloff_end();
        });

        for (auto i = 0; i < _map_lights.size(); ++i) {
            if (const auto &light = _map_lights[i];
                light.is_global()) {
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

        const auto day_half_minutes = (_time_of_day_ms / 1000 / 30) % DAY_LENGTH_HALF_MIN;

        auto fog_color = glm::vec4(0.0f);
        auto diffuse_color = glm::vec4(1.0f);
        auto ambient_color = glm::vec4(0.0f);
        auto sky_color = glm::vec4(0.0f);

        float fog_distance = 0.0f;

        auto has_one_light = false;

        for (auto i = 0; i < _map_lights.size(); ++i) {
            if (_weights[i] > 0.0f) {
                has_one_light = true;
                const auto &light = _map_lights[i];
                fog_color += light.color(light_colors::sky_fog, day_half_minutes) * _weights[i];
                diffuse_color += light.color(light_colors::diffuse, day_half_minutes) * _weights[i];
                ambient_color += light.color(light_colors::ambient, day_half_minutes) * _weights[i];
                fog_distance += light.float_value(light_float::fog_distance, day_half_minutes) / 36.0f * 2 * _weights[i];
                sky_color += light.color(light_colors::sky_band1, day_half_minutes) * _weights[i];
            }
        }

        if (!has_one_light) {
            fog_distance = 2.0 * utils::TILE_SIZE;
        }

        glClearColor(sky_color.r, sky_color.g, sky_color.b, 1.0);

        gl::mesh::terrain_mesh()
                .apply_fog_color(fog_color)
                .apply_diffuse_color(diffuse_color)
                .apply_ambient_color(ambient_color)
                .apply_fog_distance(fog_distance)
                .apply_sun_direction(calculate_sun_direction(day_half_minutes));
    }

    void light_manager::update_position(const glm::vec3 position) {
        _position = position;
        _position_changed = true;
    }

    time_t light_manager::time_of_day() const {
        const auto hours = (_time_of_day_ms / (1000 * 60 * 60)) % 24;
        const auto minutes = (_time_of_day_ms / (1000 * 60)) % 60;

        auto now = std::time(nullptr);
        auto today = *std::localtime(&now);
        today.tm_hour = static_cast<int>(hours);
        today.tm_min = static_cast<int>(minutes);
        today.tm_sec = 0;

        return std::mktime(&today);
    }
}
