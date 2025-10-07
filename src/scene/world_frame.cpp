#include "world_frame.h"

#include "utils/di.h"

namespace wow::scene {
    void world_frame::shutdown() const {
        _map_manager->shutdown();
    }

    void world_frame::enter_world(uint32_t map_id, const glm::vec2 &position) const {
        _map_manager->enter_world(map_id, position);
    }

    void world_frame::on_frame() {
        if (_camera->update()) {
            _map_manager->update(_camera->position());
        }

        _scene_info.camera_position = _camera->position();
        _scene_info.view_distance = 1.5f * utils::TILE_SIZE;

        _dispatcher->process_one_frame();
        _map_manager->on_frame(_scene_info);

        ++_frame_count;
        if (const auto now = std::chrono::steady_clock::now();
            (now - _last_fps_update) > std::chrono::seconds(1)) {
            const auto fps = _frame_count;
            _frame_count = 0;
            _last_fps_update = now;
            web::proto::JsEvent ev = {};
            ev.mutable_fps_update_event()->set_fps(fps);
            utils::app_module->ui_event_system()->event_manager()->submit(ev);
        }
    }
}
