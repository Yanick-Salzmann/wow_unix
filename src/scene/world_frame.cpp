#include "world_frame.h"

#include "utils/di.h"
#include "utils/system_stats.h"

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
        _scene_info.view_distance = 2.0f * utils::TILE_SIZE;
        gl::mesh::terrain_mesh()->program()->use();
        gl::mesh::terrain_mesh()->program()->vec4(glm::vec4(_scene_info.camera_position, _scene_info.view_distance),
                                                  "camera_position");
        gl::mesh::terrain_mesh()->program()->unuse();

        _dispatcher->process_one_frame();
        _map_manager->on_frame(_scene_info);

        ++_frame_count;
        if (const auto now = std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::seconds>(now - _last_fps_update).count() >= 1) {
            const auto fps = _frame_count;
            _frame_count = 0;
            _last_fps_update = now;
            web::proto::JsEvent ev = {};
            ev.mutable_fps_update_event()->set_fps(fps);
            utils::app_module->ui_event_system()->event_manager()->submit(ev);
        }

        if (const auto now = std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_system_update).count() >= 40) {
            _last_system_update = now;
            const auto [cpu_usage, memory_usage, total_memory, gpu_usage] = utils::get_system_stats();
            web::proto::JsEvent sys_ev = {};
            sys_ev.mutable_system_update_event()->set_memory_usage(memory_usage);
            sys_ev.mutable_system_update_event()->set_cpu_usage(cpu_usage);
            sys_ev.mutable_system_update_event()->set_gpu_usage(gpu_usage);
            sys_ev.mutable_system_update_event()->set_total_memory(total_memory);
            utils::app_module->ui_event_system()->event_manager()->submit(sys_ev);
        }
    }
}
