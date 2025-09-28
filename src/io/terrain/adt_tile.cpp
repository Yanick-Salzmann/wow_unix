#include "adt_tile.h"

#include "spdlog/spdlog.h"

namespace wow::io::terrain {
    void adt_tile::read_chunks(const utils::binary_reader_ptr &reader) {
        while (!reader->eof()) {
            const auto signature = reader->read<uint32_t>();
            const auto size = reader->read<uint32_t>();
            std::vector<uint8_t> data(size);
            reader->read(data.data(), size);
            if (signature != 'MCNK') {
                _data_chunks[signature] = {signature, size, std::move(data)};
            }
        }
    }

    bool adt_tile::load_chunk_indices() {
        if (!_data_chunks.contains('MCIN')) {
            return false;
        }

        const auto data = _data_chunks['MCIN'].data;
        if (data.size() < 256 * sizeof(decltype(_chunk_indices)::value_type)) {
            return false;
        }

        memcpy(_chunk_indices.data(), data.data(),
               _chunk_indices.size() * sizeof(decltype(_chunk_indices)::value_type));
        return true;
    }

    void adt_tile::load_textures() {
        if (!_data_chunks.contains('MTEX')) {
            return;
        }

        const auto str_data = _data_chunks['MTEX'].data;
        const auto str_ptr = reinterpret_cast<const char *>(str_data.data());
        const auto str_end = str_ptr + str_data.size();
        auto cur_offset = str_ptr;

        while (cur_offset < str_end) {
            const auto offset = std::distance(str_ptr, cur_offset);
            const auto texture_name = std::string{cur_offset};
            cur_offset += texture_name.size() + 1;
            _texture_map[offset] = _texture_manager->load(texture_name);
        }
    }

    void adt_tile::load_chunks(const utils::binary_reader_ptr &reader) {
        for (const auto &mcin: _chunk_indices) {
            reader->seek(mcin.offset);
            std::vector<uint8_t> data(mcin.size);
            reader->read(data.data(), mcin.size);
            const auto chunk = std::make_shared<adt_chunk>(utils::make_binary_reader(data));
            auto [x, y] = chunk->index();
            if (x >= 16 || y >= 16) {
                SPDLOG_WARN("Invalid chunk index {},{} in ADT tile {},{}", x, y, _x, _y);
                continue;
            }

            _chunks[x + 16 * y] = chunk;
        }
    }

    adt_tile::adt_tile(
        const uint32_t x,
        const uint32_t y,
        const utils::binary_reader_ptr &reader,
        scene::texture_manager_ptr texture_manager) : _x(x),
                                                      _y(y),
                                                      _texture_manager(std::move(texture_manager)) {
        read_chunks(reader);
        if (!_data_chunks.contains('MVER') || *reinterpret_cast<uint32_t *>(_data_chunks['MVER'].data.data()) != 0x12) {
            SPDLOG_WARN("Cannot load ADT tile {},{} - invalid version ({})", x, y,
                        *reinterpret_cast<uint32_t *>(_data_chunks['MVER'].data.data()));
            return;
        }

        if (!load_chunk_indices()) {
            SPDLOG_WARN("Cannot load ADT tile {},{} - missing/invalid MCIN chunk", x, y);
            return;
        }

        load_textures();
        load_chunks(reader);

        _data_chunks.clear();

        _async_load_successful = true;
    }

    void adt_tile::on_frame() const {
        if (!_async_load_successful || _async_unloaded) {
            return;
        }

        for (const auto &chunk: _chunks) {
            if (chunk) {
                chunk->on_frame();
            }
        }
    }

    void adt_tile::async_unload() {
        _async_unloaded = true;
        _texture_map.clear();
    }
}
