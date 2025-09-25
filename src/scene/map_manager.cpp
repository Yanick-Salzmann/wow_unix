#include "map_manager.h"

namespace wow::scene {
    void map_manager::initial_load_thread(int32_t adt_x, int32_t adt_y) {
        const auto radius = _config_manager->map().load_radius;
        for (auto ty = adt_y - radius; ty <= adt_y + radius; ++ty) {
            for (auto tx = adt_x - radius; tx <= adt_x + radius; ++tx) {
                const auto tile = fmt::format(R"(World\Maps\{}\{}_{}_{}.adt)", _directory, _directory, tx, ty);
                auto file = _mpq_manager->open(tile);
                SPDLOG_INFO("Loading ADT {} (exists: {})", tile, file != nullptr);
            }
        }
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
}
