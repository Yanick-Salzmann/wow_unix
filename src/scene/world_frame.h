#ifndef WOW_UNIX_WORLD_FRAME_H
#define WOW_UNIX_WORLD_FRAME_H
#include <memory>

#include "glm/vec2.hpp"

namespace wow::scene {
    class world_frame {
    public:
        void enter_world(uint32_t map_id, const glm::vec2 &position);
    };

    using world_frame_ptr = std::shared_ptr<world_frame>;
}

#endif //WOW_UNIX_WORLD_FRAME_H