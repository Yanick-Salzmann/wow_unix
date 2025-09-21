#include "minimap_provider.h"

#include <future>

#include "io/blp/blp_file.h"
#include "utils/string_utils.h"

namespace wow::io::minimap {
    bool minimap_provider::open_from_cache(const uint64_t cache_key, std::vector<uint8_t> &out_image) {
        std::vector<uint8_t> image{};
        {
            std::lock_guard lock(_cache_lock);
            if (const auto it = _cache.find(cache_key); it != _cache.end()) {
                image = it->second;
            }
        }

        if (image.empty()) {
            return false;
        }

        out_image = image;
        return true;
    }

    void minimap_provider::add_to_cache(uint64_t cache_key, const std::vector<uint8_t> &image_data) {
        std::lock_guard lock(_cache_lock);
        _cache[cache_key] = image_data;
    }

    blp::blp_file minimap_provider::open_tile(const uint32_t x, const uint32_t y) const {
        std::stringstream path_stream{};
        path_stream << "World\\Minimaps\\" << _base_path << "\\Map"
                << std::setw(2) << std::setfill('0') << x << "_"
                << std::setw(2) << std::setfill('0') << y << ".blp";

        return blp::blp_file{_mpq_manager->open(path_stream.str())};
    }

    minimap_provider::minimap_provider(const dbc::dbc_manager_ptr &dbc_manager,
                                       const mpq_manager_ptr &mpq_manager) : _dbc_manager(dbc_manager),
                                                                             _mpq_manager(mpq_manager) {
    }

    void minimap_provider::switch_to_map(uint32_t map_id) {
        if (_current_map_id == map_id) {
            return;
        }

        {
            std::lock_guard lock(_cache_lock);
            _cache.clear();
        }

        _base_path = utils::to_lower(_dbc_manager->map_dbc()->record(map_id).directory);
        _current_map_id = map_id;
    }

    void minimap_provider::read_image(std::vector<uint8_t> &image_data, uint32_t map_id, uint32_t zoom_level,
                                      int32_t tx, int32_t ty) {
        if (static_cast<int32_t>(map_id) < 0) {
            throw std::runtime_error("Invalid map id");
        }

        if (tx < 0 || ty < 0 || tx >= 64 || ty >= 64) {
            throw std::out_of_range("Invalid tile coordinates");
        }

        switch_to_map(map_id);

        zoom_level = std::min(zoom_level, 6u);
        const auto num_tiles = 64 >> zoom_level;
        const auto per_tile_pixels = 256 / num_tiles;

        std::vector<uint8_t> tile_data(256 * 256 * 4);

        const auto base_x = tx * num_tiles;
        const auto base_y = ty * num_tiles;

        const auto cache_key = tx + 64 * ty + zoom_level * 4096;
        if (open_from_cache(cache_key, tile_data)) {
            image_data = tile_data;
            return;
        }

        std::vector<std::shared_future<void> > futures{};
        for (auto y = 0; y < num_tiles; ++y) {
            for (auto x = 0; x < num_tiles; ++x) {
                futures.emplace_back(
                    _loader_pool.submit(
                        [this, base_x, base_y, x, y, per_tile_pixels, &tile_data]() {
                            const auto tile = open_tile(base_x + x, base_y + y);
                            uint32_t tw = 0, th = 0;
                            const auto tile_image = tile.convert_to_rgba(per_tile_pixels, tw, th);

                            auto pixel_advance = tw / per_tile_pixels;
                            auto row_advance = th / per_tile_pixels;

                            for (auto iy = 0; iy < per_tile_pixels; ++iy) {
                                for (auto ix = 0; ix < per_tile_pixels; ++ix) {
                                    const auto cur_row = row_advance * iy;
                                    const auto cur_pixel = pixel_advance * ix;

                                }
                            }
                        }
                    )
                );
            }
        }
    }
}
