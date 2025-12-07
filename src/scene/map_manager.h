#ifndef WOW_UNIX_MAP_MANAGER_H
#define WOW_UNIX_MAP_MANAGER_H
#include <memory>

#include "camera.h"
#include "config/config_manager.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "io/dbc/dbc_manager.h"
#include "io/terrain/adt_tile.h"
#include "utils/constants.h"
#include "utils/work_pool.h"
#include "scene_info.h"
#include "sky/light_manager.hpp"
#include "sky/sky_sphere.h"

namespace wow::scene {
    class map_manager {
        config::config_manager_ptr _config_manager{};
        io::dbc::dbc_manager_ptr _dbc_manager{};
        io::mpq_manager_ptr _mpq_manager{};
        texture_manager_ptr _texture_manager;
        camera_ptr _camera;

        io::terrain::wdt_file_ptr _active_wdt{};

        std::string _directory{};
        int32_t _map_id = -1;
        std::string _map_name{};

        glm::vec3 _position{};
        bool _position_changed = false;

        bool _is_running = true;
        int32_t _last_area_id = -1;

        utils::work_pool _tile_load_pool{};

        sky::sky_sphere _sky_sphere{};
        sky::light_manager_ptr _light_manager{};

        std::thread _load_thread{};
        std::mutex _async_load_lock{};
        std::mutex _sync_load_lock{};
        std::list<io::terrain::adt_tile_ptr> _async_loaded_tiles{};
        std::list<io::terrain::adt_tile_ptr> _loaded_tiles{};
        std::list<io::terrain::adt_tile_ptr> _tiles_to_unload{};

        std::atomic_int _initial_load_count = 0;
        int32_t _initial_total_load = 0;
        bool _is_initial_load_complete = false;

        void async_load_tile(uint32_t x, uint32_t y, utils::binary_reader_ptr data);

        void initial_load_thread(int32_t adt_x, int32_t adt_y);

        void handle_load_tick();

        void position_update_thread();

        void update_area_id();

    public:
        explicit map_manager(
            io::dbc::dbc_manager_ptr dbc_manager,
            config::config_manager_ptr config_manager,
            io::mpq_manager_ptr mpq_manager,
            texture_manager_ptr texture_manager,
            camera_ptr camera,
            sky::light_manager_ptr light_manager
        );

        void update(glm::vec3 position);

        bool is_initial_load_complete() const {
            return _is_initial_load_complete;
        }

        void enter_world(uint32_t map_id, const glm::vec2 &position);

        void on_frame(const scene_info& scene_info);

        void shutdown();

        void add_load_progress();

        float height(float x, float y);

        io::terrain::adt_chunk_ptr chunk_at(float x, float y);

        void initialize();
    };

    using map_manager_ptr = std::shared_ptr<map_manager>;
}

#endif //WOW_UNIX_MAP_MANAGER_H
