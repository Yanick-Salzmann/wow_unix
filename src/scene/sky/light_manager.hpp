#ifndef GLAD_LIGHT_MANAGER_HPP
#define GLAD_LIGHT_MANAGER_HPP

#include <cstdint>
#include <utility>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "io/dbc/dbc_manager.h"

namespace wow::scene::sky {
    class light_manager {
        int32_t _current_map = -1;

        std::vector<io::dbc::light_record> _map_lights{};

        io::dbc::dbc_manager_ptr _dbc_manager{};

        bool _position_changed = false;
        glm::vec3 _position{};

        glm::vec4 _fog_color{};

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
