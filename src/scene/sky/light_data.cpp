#include "light_data.hpp"

#include "glm/common.hpp"

namespace wow::scene::sky {
    glm::vec4 to_vec4(const uint32_t color) {
        return glm::vec4{
            static_cast<float>(color & 0xFF) / 255.0f,
            static_cast<float>((color >> 8) & 0xFF) / 255.0f,
            static_cast<float>((color >> 16) & 0xFF) / 255.0f,
            static_cast<float>((color >> 24) & 0xFF) / 255.0f
        };
    }

    template<typename T>
    T interpolate(const std::vector<int32_t> &times, const std::vector<T> &values, uint32_t time) {
        if (times.empty()) {
            return {};
        }

        if (time < times[0]) {
            time = times[0];
        }

        auto ts = 0;
        auto te = 0;

        auto vs = T{};
        auto ve = T{};

        for (auto i = 0; i < times.size(); ++i) {
            if (i == times.size() - 1) {
                ts = times[i];
                vs = values[i];

                te = times[0] + 2880;
                ve = values[0];
                break;
            }

            if (time >= times[i] && time <= times[i + 1]) {
                ts = times[i];
                te = times[i + 1];

                vs = values[i];
                ve = values[i + 1];
                break;
            }
        }

        auto sat = static_cast<float>(time - ts) / static_cast<float>(te - ts);
        return glm::mix(vs, ve, sat);
    }


    timed_color::timed_color(const io::dbc::light_int_band_record &record) {
        _times.resize(record.num_entries);
        _colors.resize(record.num_entries);

        for (auto i = 0; i < record.num_entries; ++i) {
            _times[i] = record.times[i];
            _colors[i] = to_vec4(record.colors[i]);
        }
    }

    glm::vec4 timed_color::value(const uint32_t time) const {
        return interpolate(_times, _colors, time);
    }

    timed_float::timed_float(const io::dbc::light_float_band_record &record) {
        _times.assign(std::begin(record.times), std::begin(record.times) + record.num_entries);
        _values.assign(std::begin(record.values), std::begin(record.values) + record.num_entries);
    }

    float timed_float::value(const uint32_t time) const {
        return interpolate(_times, _values, time);
    }

    io::dbc::light_int_band_record light_data::int_band(const io::dbc::dbc_manager_ptr &mgr, const int32_t id,
                                                        light_colors color) {
        return mgr->light_int_band_dbc()->record(id * 18 - 17 + static_cast<int32_t>(color));
    }

    io::dbc::light_float_band_record light_data::float_band(const io::dbc::dbc_manager_ptr &mgr, const int32_t id,
                                                            light_float value) {
        return mgr->light_float_band_dbc()->record(id * 6 - 5 + static_cast<int32_t>(value));
    }

    light_data::light_data(const io::dbc::dbc_manager_ptr &mgr, const io::dbc::light_record &light) : _light{light} {
        _colors.emplace(light_colors::diffuse, timed_color(int_band(mgr, light.params_clear, light_colors::diffuse)));
        _colors.emplace(light_colors::ambient, timed_color(int_band(mgr, light.params_clear, light_colors::ambient)));
        _colors.emplace(light_colors::sky_top, timed_color(int_band(mgr, light.params_clear, light_colors::sky_top)));
        _colors.emplace(light_colors::sky_middle, timed_color(int_band(mgr, light.params_clear, light_colors::sky_middle)));
        _colors.emplace(light_colors::sky_band1, timed_color(int_band(mgr, light.params_clear, light_colors::sky_band1)));
        _colors.emplace(light_colors::sky_band2, timed_color(int_band(mgr, light.params_clear, light_colors::sky_band2)));
        _colors.emplace(light_colors::sky_smog, timed_color(int_band(mgr, light.params_clear, light_colors::sky_smog)));
        _colors.emplace(light_colors::sky_fog, timed_color(int_band(mgr, light.params_clear, light_colors::sky_fog)));
        _colors.emplace(light_colors::sun_color, timed_color(int_band(mgr, light.params_clear, light_colors::sun_color)));

        _floats.emplace(light_float::fog_distance, timed_float(float_band(mgr, light.params_clear, light_float::fog_distance)));
        _floats.emplace(light_float::fog_multiplier,
                        timed_float(float_band(mgr, light.params_clear, light_float::fog_multiplier)));
    }
}
