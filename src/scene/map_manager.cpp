#include "map_manager.h"

namespace wow::scene {
    void map_manager::async_load_tile(uint32_t x, uint32_t y, utils::binary_reader_ptr data) {
        const auto adt = std::make_shared<io::terrain::adt_tile>(x, y, data, _texture_manager);
        SPDLOG_INFO("I/O loaded ADT tile {},{}", x, y);
        std::lock_guard lock(_async_load_lock);
        _async_loaded_tiles.push_back(adt);
    }

    void map_manager::initial_load_thread(const int32_t adt_x, const int32_t adt_y) {
        const auto radius = _config_manager->map().load_radius;
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

        std::thread{&map_manager::initial_load_thread, this, start_adt, end_adt}.detach();
    }

    void map_manager::shutdown() {
        _async_loaded_tiles.clear();
    }
}
