#ifndef WOW_UNIX_ADT_TILE_H
#define WOW_UNIX_ADT_TILE_H

#include <map>
#include <memory>

#include "adt_chunk.h"
#include "wdt_file.h"
#include "scene/texture_manager.h"
#include "utils/io.h"
#include "utils/math.h"

namespace wow::io::terrain {
    class adt_tile {
        struct data_chunk {
            uint32_t signature;
            uint32_t size;
            std::vector<uint8_t> data;
        };

#pragma pack(push, 1)
        struct chunk_info {
            uint32_t offset;
            uint32_t size;
            uint32_t flags;
            uint32_t padding;
        };
#pragma pack(pop)

        uint32_t _x{};
        uint32_t _y{};

        utils::bounding_box _bounds{};

        std::atomic_bool _async_load_successful = false;
        std::atomic_bool _async_unloaded = false;
        std::map<uint32_t, data_chunk> _data_chunks{};

        scene::texture_manager_ptr _texture_manager;

        std::map<uint32_t, gl::texture_ptr> _texture_map{};

        std::array<chunk_info, 256> _chunk_indices{};

        std::array<adt_chunk_ptr, 256> _chunks{};

        void read_chunks(const utils::binary_reader_ptr &reader);

        bool load_chunk_indices();

        void load_textures();

        void load_chunks(wdt_file_ptr wdt, const utils::binary_reader_ptr &reader);

    public:
        adt_tile(
            wdt_file_ptr wdt,
            uint32_t x,
            uint32_t y,
            const utils::binary_reader_ptr &reader,
            scene::texture_manager_ptr texture_manager
        );

        ~adt_tile() {
            if (!_async_unloaded && _async_load_successful) {
                async_unload();
            }
        }

        void on_frame() const;

        void async_unload();

        uint32_t x() const {
            return _x;
        }

        uint32_t y() const {
            return _y;
        }

        adt_chunk_ptr chunk(uint32_t index) const {
            if (index >= _chunks.size()) {
                return nullptr;
            }

            return _chunks[index];
        }
    };

    using adt_tile_ptr = std::shared_ptr<adt_tile>;
}

#endif //WOW_UNIX_ADT_TILE_H
