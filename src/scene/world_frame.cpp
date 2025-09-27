#include "world_frame.h"

namespace wow::scene {
    void world_frame::shutdown() const {
        _map_manager->shutdown();
    }

    void world_frame::enter_world(uint32_t map_id, const glm::vec2 &position) const {
        _map_manager->enter_world(map_id, position);
    }

    void world_frame::on_frame() const {
        _dispatcher->process_one_frame();
    }
}
