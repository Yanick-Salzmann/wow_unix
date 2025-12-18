#include "world_frame.h"

#include "utils/di.h"
#include "utils/system_stats.h"

namespace wow::scene {
void world_frame::handle_metrics() {
  if (const auto now = std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_system_update).count() >= 40) {
    _last_system_update = now;
    const auto [cpu_usage, memory_usage, total_memory, gpu_usage, cpu_freq,
                gpu_mem_used, gpu_mem_total] = utils::get_system_stats();
    web::proto::JsEvent sys_ev = {};
    sys_ev.mutable_system_update_event()->set_memory_usage(memory_usage);
    sys_ev.mutable_system_update_event()->set_cpu_usage(cpu_usage);
    sys_ev.mutable_system_update_event()->set_gpu_usage(gpu_usage);
    sys_ev.mutable_system_update_event()->set_total_memory(total_memory);
    sys_ev.mutable_system_update_event()->set_cpu_frequency_mhz(cpu_freq);
    sys_ev.mutable_system_update_event()->set_gpu_memory_used(gpu_mem_used);
    sys_ev.mutable_system_update_event()->set_gpu_memory_total(gpu_mem_total);
    utils::app_module->ui_event_system()->event_manager()->submit(sys_ev);
  }
}

void world_frame::handle_fps_update() {
  ++_frame_count;
  if (const auto now = std::chrono::steady_clock::now();
      std::chrono::duration_cast<std::chrono::seconds>(now - _last_fps_update)
          .count() >= 1) {
    const auto fps = _frame_count;
    _frame_count = 0;
    _last_fps_update = now;
    web::proto::JsEvent ev = {};
    ev.mutable_fps_update_event()->set_fps(fps);
    ev.mutable_fps_update_event()->set_time_of_day(_map_manager->time_of_day());
    utils::app_module->ui_event_system()->event_manager()->submit(ev);
  }
}

    void world_frame::initialize() {
        _map_manager->initialize();
    }

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

  _dispatcher->process_one_frame();

  _scene_info.camera_position = _camera->position();
  _scene_info.view_distance = 2.0f * utils::TILE_SIZE;

  const auto mesh = gl::mesh::terrain_mesh().mesh;

  if (_camera_position_uniform < 0) {
            _camera_position_uniform = mesh->program()->uniform_location("camera_position");
  }

  mesh->apply_blend_mode();
  mesh->bind_vertex_attributes();
  mesh->program()->use();
        mesh->program()->vec4(glm::vec4(_scene_info.camera_position, _scene_info.view_distance),
      _camera_position_uniform);

  if (const auto &ib = io::terrain::adt_chunk::index_buffer()) {
    ib->bind();
  }

  _map_manager->on_frame(_scene_info);

  handle_fps_update();
  handle_metrics();
}
}
