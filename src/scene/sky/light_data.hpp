#ifndef GLAD_LIGHT_DATA_HPP
#define GLAD_LIGHT_DATA_HPP

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "io/dbc/dbc_manager.h"

namespace wow::scene::sky {
    class timed_color {
        std::vector<int32_t> _times{};
        std::vector<glm::vec4> _colors{};
    public:
        explicit timed_color(const io::dbc::light_int_band_record &record);

        [[nodiscard]] glm::vec4 value(uint32_t time) const;
    };

    class interpolated_float {
        std::vector<int32_t> _times{};
        std::vector<float> _values{};

    public:
        explicit interpolated_float(const io::dbc::light_float_band_record& record);

        [[nodiscard]] float value(uint32_t time) const;
    };

    enum class light_colors {
        diffuse = 0,
        ambient,
        sky_top,
        sky_middle,
        sky_band1,
        sky_band2,
        sky_smog,
        sky_fog,
        sun_color,
    };

    enum class light_float {
        fog_distance,
        fog_multiplier
    };

    class light_data {
        io::dbc::light_record _light{};
        std::unordered_map<light_colors, timed_color> _colors{};
        std::unordered_map<light_float, interpolated_float> _floats{};

        static io::dbc::light_int_band_record int_band(const io::dbc::dbc_manager_ptr& mgr, int32_t id, light_colors color);
        static io::dbc::light_float_band_record float_band(const io::dbc::dbc_manager_ptr& mgr, int32_t id, light_float value);

    public:
        light_data(const io::dbc::dbc_manager_ptr& mgr, const io::dbc::light_record &light);

        bool is_global() const {
            return _light.x == 0 && _light.y == 0 && _light.z == 0;
        }

        glm::vec4 color(light_colors color, uint32_t time) const {
            return _colors.at(color).value(time);
        }

        glm::vec3 position() const {
            return glm::vec3{_light.x / 36.0f, _light.z / 36.0f, _light.y / 36.0f};
        }

        float falloff_start() const {
            return _light.falloff_start / 36.0f;
        }

        float falloff_end() const {
            return _light.falloff_end / 36.0f;
        }
    };
}

#endif //GLAD_LIGHT_DATA_HPP
