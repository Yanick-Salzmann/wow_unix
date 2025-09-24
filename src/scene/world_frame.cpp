#include "world_frame.h"

#include "utils/log_utils.h"
#include "spdlog/spdlog.h"

namespace wow::scene {
    void world_frame::enter_world(uint32_t map_id, const glm::vec2 &position) {
        _map_manager->enter_world(map_id, position);
    }
}
