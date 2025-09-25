#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include <memory>
#include <toml++/toml.h>

namespace wow::config {
    struct map_config {
        int32_t load_radius{};
    };

    class config_manager {
        toml::table _config{};

        map_config _map_config{};

        static int32_t int_value(const toml::table& obj, const std::string &key, int32_t default_value);

        int32_t int_value(const std::string &key, int32_t default_value);
        int32_t int_value(const std::string& section, const std::string &key, int32_t default_value);

    public:
        config_manager();

        [[nodiscard]] const map_config &map() const {
            return _map_config;
        }
    };

    using config_manager_ptr = std::shared_ptr<config_manager>;
}
