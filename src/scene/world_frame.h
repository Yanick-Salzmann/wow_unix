#ifndef WOW_UNIX_WORLD_FRAME_H
#define WOW_UNIX_WORLD_FRAME_H
#include <memory>

#include "gpu_dispatcher.h"
#include "map_manager.h"
#include "glm/vec2.hpp"

namespace wow::scene {
    class world_frame {
        map_manager_ptr _map_manager{};
        gpu_dispatcher_ptr _dispatcher{};

    public:
        explicit world_frame(
            map_manager_ptr map_manager,
            gpu_dispatcher_ptr dispatcher
        ) : _map_manager(std::move(map_manager)),
            _dispatcher(std::move(dispatcher)) {
        }

        void enter_world(uint32_t map_id, const glm::vec2 &position);

        void on_frame() const;
    };

    using world_frame_ptr = std::shared_ptr<world_frame>;
}

#endif //WOW_UNIX_WORLD_FRAME_H
