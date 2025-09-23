#include "world_frame.h"

#include "utils/log_utils.h"
#include "spdlog/spdlog.h"

namespace wow::scene {
    void world_frame::enter_world(uint32_t map_id, const glm::vec2 &position) {
        SPDLOG_INFO("Entering map {} at {}", map_id, position);
    }
}
