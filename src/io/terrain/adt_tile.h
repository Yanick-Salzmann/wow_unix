#ifndef WOW_UNIX_ADT_TILE_H
#define WOW_UNIX_ADT_TILE_H
#include <map>
#include <memory>

#include "adt_chunk.h"
#include "scene/texture_manager.h"
#include "utils/io.h"

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

        bool _async_load_successful = false;

        scene::texture_manager_ptr _texture_manager;

        std::map<uint32_t, gl::texture_ptr> _texture_map{};

        std::map<uint32_t, data_chunk> _data_chunks{};
        std::vector<chunk_info> _chunk_indices{};

        std::vector<adt_chunk_ptr> _chunks{};

        void read_chunks(const utils::binary_reader_ptr &reader);

        bool load_chunk_indices();
        void load_textures();

    public:
        adt_tile(
            uint32_t x,
            uint32_t y,
            const utils::binary_reader_ptr &reader,
            scene::texture_manager_ptr texture_manager
        );

        void async_unload();
    };

    using adt_tile_ptr = std::shared_ptr<adt_tile>;
}

#endif //WOW_UNIX_ADT_TILE_H
