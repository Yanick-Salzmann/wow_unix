#ifndef WOW_UNIX_WORLD_FRAME_H
#define WOW_UNIX_WORLD_FRAME_H
#include <memory>

#include "map_manager.h"
#include "glm/vec2.hpp"

namespace wow::scene {
    class world_frame {
        map_manager_ptr _map_manager{};

    public:
        explicit world_frame(map_manager_ptr map_manager) : _map_manager(std::move(map_manager)) {
        }

        void enter_world(uint32_t map_id, const glm::vec2 &position);
    };

    using world_frame_ptr = std::shared_ptr<world_frame>;
}

#endif //WOW_UNIX_WORLD_FRAME_H
