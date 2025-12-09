#ifndef GLAD_LIGHT_MANAGER_HPP
#define GLAD_LIGHT_MANAGER_HPP

#include <cstdint>
#include <utility>
#include <vector>

#include "light_data.hpp"
#include "glm/common.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "io/dbc/dbc_manager.h"

namespace wow::scene::sky {
    class light_manager {
        int32_t _current_map = -1;

        std::vector<light_data> _map_lights{};
        std::vector<int32_t> _global_lights{};
        std::vector<float> _weights{};

        io::dbc::dbc_manager_ptr _dbc_manager{};

        std::chrono::steady_clock::time_point _last_update{};
        uint64_t _time_of_day_ms = 0;

        bool _position_changed = false;
        glm::vec3 _position{};

        static glm::vec3 calculate_sun_direction(uint32_t day_half_minutes);

        void update_weights();

    public:
        explicit light_manager(io::dbc::dbc_manager_ptr dbc_mgr) : _dbc_manager(std::move(dbc_mgr)) {
        }

        void enter_world(int32_t map_id);

        void on_update();

        void update_position(glm::vec3 position);
    };

    typedef std::shared_ptr<light_manager> light_manager_ptr;
}

#endif //GLAD_LIGHT_MANAGER_HPP
