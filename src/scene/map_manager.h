#ifndef WOW_UNIX_MAP_MANAGER_H
#define WOW_UNIX_MAP_MANAGER_H
#include <memory>

#include "config/config_manager.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "io/dbc/dbc_manager.h"
#include "io/terrain/adt_tile.h"
#include "utils/constants.h"
#include "utils/work_pool.h"

namespace wow::scene {
    class map_manager {
        config::config_manager_ptr _config_manager{};
        io::dbc::dbc_manager_ptr _dbc_manager{};
        io::mpq_manager_ptr _mpq_manager{};

        std::string _directory{};

        glm::vec3 _position{};

        utils::work_pool _tile_load_pool{};

        std::mutex _async_load_lock{};
        std::list<io::terrain::adt_tile_ptr> _async_loaded_tiles{};
        std::map<uint32_t, io::terrain::adt_tile_ptr> _loaded_tiles{};

        void async_load_tile(uint32_t x, uint32_t y, utils::binary_reader_ptr data);

        void initial_load_thread(int32_t adt_x, int32_t adt_y);

    public:
        explicit map_manager(
            io::dbc::dbc_manager_ptr dbc_manager,
            config::config_manager_ptr config_manager,
            io::mpq_manager_ptr mpq_manager
        ) : _config_manager(std::move(config_manager)),
            _dbc_manager(std::move(dbc_manager)),
            _mpq_manager(std::move(mpq_manager)) {
        }

        void enter_world(uint32_t map_id, const glm::vec2 &position);
    };

    using map_manager_ptr = std::shared_ptr<map_manager>;
}

#endif //WOW_UNIX_MAP_MANAGER_H
