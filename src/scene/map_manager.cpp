#include "map_manager.h"

#include "utils/di.h"

namespace wow::scene {
    void map_manager::async_load_tile(uint32_t x, uint32_t y, utils::binary_reader_ptr data) {
        const auto adt = std::make_shared<io::terrain::adt_tile>(x, y, data, _texture_manager);
        SPDLOG_INFO("I/O loaded ADT tile {},{}", x, y);
        std::lock_guard lock(_async_load_lock);
        _async_loaded_tiles.push_back(adt);
    }

    void map_manager::initial_load_thread(const int32_t adt_x, const int32_t adt_y) {
        const auto radius = 1; //_config_manager->map().load_radius;
        std::vector<std::shared_future<void> > futures{};

        for (auto ty = adt_y - radius; ty <= adt_y + radius; ++ty) {
            for (auto tx = adt_x - radius; tx <= adt_x + radius; ++tx) {
                const auto tile = fmt::format(R"(World\Maps\{}\{}_{}_{}.adt)", _directory, _directory, tx, ty);
                const auto file = _mpq_manager->open(tile);
                if (!file) {
                    SPDLOG_DEBUG("Not loading ADT tile {},{} for map {} - file not found", tx, ty, _directory);
                    continue;
                }

                auto reader = file->to_binary_reader();
                futures.push_back(_tile_load_pool.submit([this, tx, ty, reader] { async_load_tile(tx, ty, reader); }));
            }
        }

        std::ranges::for_each(futures, [](const auto &f) { f.get(); });
        _camera->enter_world(glm::vec3{_position.x, _position.y, 100.0f});
    }

    void map_manager::enter_world(uint32_t map_id, const glm::vec2 &position) {
        const auto rec = _dbc_manager->map_dbc()->record(static_cast<int32_t>(map_id));
        _directory = rec.directory;
        if (_directory.empty()) {
            SPDLOG_ERROR("Invalid map id {}", map_id);
            return;
        }

        _position = glm::vec3(position, 0.0f);

        int32_t start_adt = position.x / utils::TILE_SIZE;
        int32_t end_adt = position.y / utils::TILE_SIZE;

        SPDLOG_INFO("Entering map {} ({}) at {},{} (adt {} {})", map_id, _directory, position.x, position.y, start_adt,
                    end_adt);

        web::proto::JsEvent enter_event{};
        enter_event.mutable_loading_screen_show_event()->set_image_path("");
        utils::app_module->ui_event_system()->event_manager()->submit(enter_event);

        std::thread{&map_manager::initial_load_thread, this, start_adt, end_adt}.detach();
    }

    void map_manager::on_frame() {
        if (!_async_loaded_tiles.empty()) {
            std::lock_guard lock(_async_load_lock);
            _loaded_tiles.insert(_loaded_tiles.end(), _async_loaded_tiles.begin(), _async_loaded_tiles.end());
            _async_loaded_tiles.clear();
        }

        for (const auto &tile: _loaded_tiles) {
            tile->on_frame();
        }
    }

    void map_manager::shutdown() {
        _async_loaded_tiles.clear();
        _loaded_tiles.clear();
    }

    float map_manager::height(float x, float y) {
        const auto tx = static_cast<int32_t>(x / utils::TILE_SIZE);
        const auto ty = static_cast<int32_t>(y / utils::TILE_SIZE);
        if (tx < 0 || ty < 0 || tx >= 64 || ty >= 64) {
            return 0.0f;
        }

        const auto cx = static_cast<int32_t>((x - static_cast<float>(tx) * utils::TILE_SIZE) / utils::CHUNK_SIZE);
        const auto cy = static_cast<int32_t>((y - static_cast<float>(ty) * utils::TILE_SIZE) / utils::CHUNK_SIZE);

        if (cx < 0 || cy < 0 || cx >= 16 || cy >= 16) {
            return 0.0f;
        }

        return 0.0f;
    }
}
