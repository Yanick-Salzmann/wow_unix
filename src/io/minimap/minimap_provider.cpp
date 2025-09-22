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
        path_stream << _base_path << "\\Map"
                << x << "_"
                << std::setw(2) << std::setfill('0') << y << ".blp";

        if (const auto key = utils::to_lower(path_stream.str()); _md5_translate.contains(key)) {
            return blp::blp_file{_mpq_manager->open(fmt::format("textures\\minimap\\{}", _md5_translate.at(key)))};
        }

        return blp::blp_file{nullptr};
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

            if (_md5_translate.empty()) {
                auto fl = _mpq_manager->open("textures\\minimap\\md5translate.trs");
                auto content = fl->read_text();

                std::stringstream reader{};
                reader << content;

                std::string line{};
                while (std::getline(reader, line)) {
                    if (line.substr(0, 4) == "dir:") {
                        continue;
                    }

                    std::stringstream line_reader{};
                    line_reader << line;
                    std::string md5{}, file{};
                    line_reader >> file >> md5;
                    _md5_translate[utils::to_lower(file)] = md5;
                }
            }
        }

        _base_path = utils::to_lower(_dbc_manager->map_dbc()->record(map_id).directory);
        _current_map_id = map_id;
    }

    void minimap_provider::read_image(std::vector<uint8_t> &image_data, const uint32_t map_id, uint32_t zoom_level,
                                      const int32_t tx, const int32_t ty) {
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
        auto tile_ptr = reinterpret_cast<uint32_t *>(tile_data.data());

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
                        [this, base_x, base_y, x, y, per_tile_pixels, tile_ptr]() {
                            const auto tile = open_tile(base_x + x, base_y + y);
                            uint32_t tw = 0, th = 0;
                            const auto tile_image = tile.convert_to_rgba(per_tile_pixels, tw, th);
                            const auto img_ptr = reinterpret_cast<const uint32_t *>(tile_image.data());
                            const std::vector img_small(img_ptr, img_ptr + tile_image.size() / 4);

                            const auto pixel_advance = tw / per_tile_pixels;
                            const auto row_advance = th / per_tile_pixels;

                            for (auto iy = 0; iy < per_tile_pixels; ++iy) {
                                for (auto ix = 0; ix < per_tile_pixels; ++ix) {
                                    const auto cur_row = row_advance * iy;
                                    const auto cur_pixel = pixel_advance * ix;
                                    const auto ofs_tile_image = cur_row * tw + cur_pixel;

                                    const auto target_row = y * per_tile_pixels;
                                    const auto target_pixel = x * per_tile_pixels;
                                    const auto ofs_tile_data = (target_row + iy) * 256 + (target_pixel + ix);
                                    *(tile_ptr + ofs_tile_data) = img_small[ofs_tile_image];
                                }
                            }
                        }
                    )
                );
            }
        }

        std::ranges::for_each(futures, [](auto &f) { f.get(); });
        image_data = utils::to_png(tile_data, 256, 256);
        add_to_cache(cache_key, image_data);
    }
}
