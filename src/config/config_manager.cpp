#include "config_manager.h"
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

namespace wow::config {
    int32_t config_manager::int_value(const std::string &key, int32_t default_value) {
        return int_value(_config, key, default_value);
    }

    int32_t config_manager::int_value(const toml::table &obj, const std::string &key, int32_t default_value) {
        if (const auto value = obj[key].value<int32_t>(); value.has_value()) {
            return value.value();
        }

        SPDLOG_DEBUG("Config key '{}' not found, using default value: {}", key, default_value);
        return default_value;
    }

    int32_t config_manager::int_value(const std::string &section, const std::string &key, int32_t default_value) {
        const auto child = _config[section];
        return int_value(*child.as_table(), key, default_value);
    }

    config_manager::config_manager() {
        std::ifstream file{"config.toml"};
        _config = toml::parse(file, std::string_view{"config.toml"});
        _map_config.load_radius = int_value("map", "loading-radius", 3);
    }
}
