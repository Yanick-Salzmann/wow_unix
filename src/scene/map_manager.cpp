#include "map_manager.h"

#include "utils/di.h"
#include "utils/string_utils.h"

namespace wow::scene {
    void map_manager::async_load_tile(uint32_t x, uint32_t y, utils::binary_reader_ptr data) {
        const auto adt = std::make_shared<io::terrain::adt_tile>(_active_wdt, x, y, data, _texture_manager);
        adt->async_load();

        std::lock_guard lock(_async_load_lock);
        _async_loaded_tiles.push_back(adt);
        add_load_progress();
    }

    void map_manager::initial_load_thread(const int32_t adt_x, const int32_t adt_y) {
        const auto wdt_name = fmt::format(R"(World\Maps\{}\{}.wdt)", _directory, _directory);
        const auto file = _mpq_manager->open(wdt_name);
        if (!file) {
            SPDLOG_ERROR("Failed to open WDT file {}", wdt_name);
            return;
        }

        _active_wdt = io::terrain::make_wdt(file->to_binary_reader());

        const auto radius = _config_manager->map().load_radius;
        std::vector<std::shared_future<void> > futures{};

        _initial_load_count = 0;
        _initial_total_load = (radius * 2 + 1) * (radius * 2 + 1) * 257;

        for (auto ty = adt_y - radius; ty <= adt_y + radius; ++ty) {
            for (auto tx = adt_x - radius; tx <= adt_x + radius; ++tx) {
                const auto tile = fmt::format(R"(World\Maps\{}\{}_{}_{}.adt)", _directory, _directory, tx, ty);
                const auto file = _mpq_manager->open(tile);
                if (!file) {
                    add_load_progress();
                    SPDLOG_DEBUG("Not loading ADT tile {},{} for map {} - file not found", tx, ty, _directory);
                    continue;
                }

                auto reader = file->to_binary_reader();
                futures.push_back(_tile_load_pool.submit([this, tx, ty, reader] { async_load_tile(tx, ty, reader); }));
            }
        }

        std::ranges::for_each(futures, [](const auto &f) { f.get(); });
        _camera->enter_world(glm::vec3{_position.x, _position.y, 200.0f});
    }

    void map_manager::handle_load_tick() {
        if (!_async_loaded_tiles.empty()) {
            std::lock_guard lock(_async_load_lock);
            _loaded_tiles.insert(_loaded_tiles.end(), _async_loaded_tiles.begin(), _async_loaded_tiles.end());
            _async_loaded_tiles.clear();
        }

        if (_initial_total_load > 0 && _initial_load_count >= _initial_total_load) {
            web::proto::JsEvent ev = {};
            ev.mutable_loading_screen_complete_event();
            utils::app_module->ui_event_system()->event_manager()->submit(ev);
            _initial_total_load = 0;
            _is_initial_load_complete = true;
            _position.z = height(_position.x, _position.y);
            if (_position.z == -std::numeric_limits<float>::infinity()) {
                _position.z = 200.0f;
            } else {
                _position.z += 200.0f;
            }

            _camera->update_position(_position);
            SPDLOG_INFO("Entered world {} ({}) at ({}, {}, {})", _map_id, _directory, _position.x, _position.y,
                        _position.z);
        }
    }

    void map_manager::position_update_thread() {
        auto last_update = std::chrono::steady_clock::now();

        while (_is_running) {
            if (!_is_initial_load_complete || !_position_changed) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }

            _position_changed = false;

            const auto tx = static_cast<int32_t>(_position.x / utils::TILE_SIZE);
            const auto ty = static_cast<int32_t>(_position.y / utils::TILE_SIZE);

            const auto tmp = _loaded_tiles;

            std::unordered_set<int32_t> wanted_indices{};
            for (auto x = tx - _config_manager->map().load_radius; x <= tx + _config_manager->map().load_radius; ++x) {
                for (auto y = ty - _config_manager->map().load_radius; y <= ty + _config_manager->map().load_radius; ++
                     y) {
                    wanted_indices.insert(y * 64 + x);
                }
            }

            std::list<io::terrain::adt_tile_ptr> new_tiles{};
            for (const auto &tile: tmp) {
                const auto dx = tx - static_cast<int32_t>(tile->x());
                const auto dy = ty - static_cast<int32_t>(tile->y());
                if (dx > _config_manager->map().load_radius || dy > _config_manager->map().load_radius) {
                    tile->async_unload();
                    std::lock_guard lock(_sync_load_lock);
                    _tiles_to_unload.push_back(tile);
                    continue;
                }

                wanted_indices.erase(tile->y() * 64 + tile->x());
                std::lock_guard lock(_sync_load_lock);
                new_tiles.push_back(tile);
            }

            {
                std::lock_guard lock(_sync_load_lock);
                _loaded_tiles = new_tiles;
            }

            for (const auto &index: wanted_indices) {
                const auto x = index % 64;
                const auto y = index / 64;
                const auto tile = fmt::format(R"(World\Maps\{}\{}_{}_{}.adt)", _directory, _directory, x, y);
                const auto file = _mpq_manager->open(tile);
                if (!file) {
                    SPDLOG_DEBUG("Not loading ADT tile {},{} for map {} - file not found", x, y, _directory);
                    continue;
                }

                const auto reader = file->to_binary_reader();
                async_load_tile(x, y, reader);
            }

            update_area_id();

            const auto now = std::chrono::steady_clock::now();
            if (const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);
                elapsed.count() < 100) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100 - elapsed.count()));
            }

            last_update = now;
        }
    }

    void map_manager::update_area_id() {
        if (const auto cnk = chunk_at(_position.x, _position.y)) {
            const auto area_id = cnk->area_id();
            const auto do_update = area_id != _last_area_id;
            std::string area_name{"Unknown"};

            if (const auto area_dbc = _dbc_manager->area_table_dbc();
                area_dbc->has_record(area_id)) {
                area_name = area_dbc->record(area_id).name.text;
            }

            if (do_update) {
                web::proto::JsEvent ev{};
                ev.mutable_area_update_event()->set_area_name(area_name);
                ev.mutable_area_update_event()->set_area_id(area_id);
                utils::app_module->ui_event_system()->event_manager()->submit(ev);
                _last_area_id = area_id;
            }
        }

        web::proto::JsEvent ev{};
        const auto pos_ev = ev.mutable_world_position_update_event();
        pos_ev->set_map_id(_map_id);
        pos_ev->set_map_name(_map_name);
        pos_ev->set_x(_position.x);
        pos_ev->set_y(_position.y);
        pos_ev->set_z(_position.z);
        utils::app_module->ui_event_system()->event_manager()->submit(ev);
    }

    map_manager::map_manager(io::dbc::dbc_manager_ptr dbc_manager, config::config_manager_ptr config_manager,
                             io::mpq_manager_ptr mpq_manager, texture_manager_ptr texture_manager,
                             camera_ptr camera) : _config_manager(std::move(config_manager)),
                                                  _dbc_manager(std::move(dbc_manager)),
                                                  _mpq_manager(std::move(mpq_manager)),
                                                  _texture_manager(std::move(texture_manager)),
                                                  _camera(std::move(camera)) {
        _load_thread = std::thread{&map_manager::position_update_thread, this};
    }

    void map_manager::update(const glm::vec3 position) {
        _position = position;
        _position_changed = true;
    }

    void map_manager::enter_world(uint32_t map_id, const glm::vec2 &position) {
        _map_id = -1;
        _map_name.clear();

        const auto rec = _dbc_manager->map_dbc()->record(static_cast<int32_t>(map_id));
        _directory = rec.directory;
        if (_directory.empty()) {
            SPDLOG_ERROR("Invalid map id {}", map_id);
            return;
        }

        _map_id = static_cast<int32_t>(map_id);
        _map_name = rec.name.text;

        _position = glm::vec3(position, 0.0f);

        const auto start_adt = static_cast<int32_t>(position.x / utils::TILE_SIZE);
        const auto end_adt = static_cast<int32_t>(position.y / utils::TILE_SIZE);

        SPDLOG_INFO("Entering map {} ({}) at {},{} (adt {} {})", map_id, _directory, position.x, position.y, start_adt,
                    end_adt);

        const auto dbc_ls = _dbc_manager->loading_screen_dbc();

        std::string loading_screen{"blp://localhost/Interface/Glues/loading.blp"};
        if (dbc_ls->has_record(rec.loading_screen)) {
            loading_screen = "blp://localhost/" +
                             utils::replace_all(dbc_ls->record(rec.loading_screen).path, "\\", "/");
        }

        web::proto::JsEvent enter_event{};
        enter_event.mutable_loading_screen_show_event()->set_image_path(loading_screen);
        utils::app_module->ui_event_system()->event_manager()->submit(enter_event);

        std::thread{&map_manager::initial_load_thread, this, start_adt, end_adt}.detach();
    }

    void map_manager::on_frame(const scene_info& scene_info) {
        handle_load_tick();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        std::list<io::terrain::adt_tile_ptr> to_render{};
        {
            std::lock_guard lock(_sync_load_lock);
            to_render.insert(to_render.end(), _loaded_tiles.begin(), _loaded_tiles.end());
            _tiles_to_unload.clear();
        }

        for (const auto &tile: to_render) {
            tile->on_frame(scene_info);
        }

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    void map_manager::shutdown() {
        _is_running = false;
        _load_thread.join();

        _async_loaded_tiles.clear();
        _loaded_tiles.clear();
    }

    void map_manager::add_load_progress() {
        ++_initial_load_count;
        auto ev = web::proto::JsEvent{};
        ev.mutable_loading_screen_progress_event()->set_percentage(
            static_cast<float>(_initial_load_count) / static_cast<float>(_initial_total_load));
        utils::app_module->ui_event_system()->event_manager()->submit(ev);
    }

    float map_manager::height(const float x, const float y) {
        auto ox = x;
        auto oy = y;

        const auto tx = static_cast<int32_t>(x / utils::TILE_SIZE);
        const auto ty = static_cast<int32_t>(y / utils::TILE_SIZE);
        if (tx < 0 || ty < 0 || tx >= 64 || ty >= 64) {
            return -std::numeric_limits<float>::infinity();
        }

        ox -= static_cast<float>(tx) * utils::TILE_SIZE;
        oy -= static_cast<float>(ty) * utils::TILE_SIZE;

        const auto cx = static_cast<int32_t>(ox / utils::CHUNK_SIZE);
        const auto cy = static_cast<int32_t>(oy / utils::CHUNK_SIZE);

        const auto cnk = chunk_at(x, y);
        if (!cnk) {
            return -std::numeric_limits<float>::infinity();
        }

        ox -= static_cast<float>(cx) * utils::CHUNK_SIZE;
        oy -= static_cast<float>(cy) * utils::CHUNK_SIZE;

        return cnk->height(ox, oy);
    }

    io::terrain::adt_chunk_ptr map_manager::chunk_at(const float x, const float y) {
        const auto tx = static_cast<int32_t>(x / utils::TILE_SIZE);
        const auto ty = static_cast<int32_t>(y / utils::TILE_SIZE);
        if (tx < 0 || ty < 0 || tx >= 64 || ty >= 64) {
            return nullptr;
        }

        const auto cx = static_cast<int32_t>((x - static_cast<float>(tx) * utils::TILE_SIZE) / utils::CHUNK_SIZE);
        const auto cy = static_cast<int32_t>((y - static_cast<float>(ty) * utils::TILE_SIZE) / utils::CHUNK_SIZE);

        if (cx < 0 || cy < 0 || cx >= 16 || cy >= 16) {
            return nullptr;
        }

        std::list<io::terrain::adt_tile_ptr> tiles{};
        {
            std::lock_guard lock(_sync_load_lock);
            tiles = _loaded_tiles;
        }

        for (const auto &tile: tiles) {
            if (tile->x() == tx && tile->y() == ty) {
                return tile->chunk(cx + cy * 16);
            }
        }

        return nullptr;
    }
}
