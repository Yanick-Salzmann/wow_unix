#ifndef WOW_UNIX_WORLD_FRAME_H
#define WOW_UNIX_WORLD_FRAME_H
#include <memory>

#include "camera.h"
#include "gpu_dispatcher.h"
#include "map_manager.h"
#include "scene_info.h"

namespace wow::scene {
    class world_frame {
        map_manager_ptr _map_manager{};
        gpu_dispatcher_ptr _dispatcher{};
        camera_ptr _camera{};

        scene_info _scene_info{};

        int32_t _camera_position_uniform = -1;

        uint32_t _frame_count = 0;
        std::chrono::steady_clock::time_point _last_fps_update = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point _last_system_update = std::chrono::steady_clock::now();

        void handle_metrics();
        void handle_fps_update();

        void initialize();

    public:
        explicit world_frame(
            map_manager_ptr map_manager,
            gpu_dispatcher_ptr dispatcher,
            camera_ptr camera
        ) : _map_manager(std::move(map_manager)),
            _dispatcher(std::move(dispatcher)),
            _camera(std::move(camera)) {
            initialize();
        }

        void shutdown() const;

        void enter_world(uint32_t map_id, const glm::vec2 &position) const;

        void on_frame();
    };

    using world_frame_ptr = std::shared_ptr<world_frame>;
}

#endif //WOW_UNIX_WORLD_FRAME_H
