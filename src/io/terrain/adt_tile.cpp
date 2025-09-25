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

        _chunk_indices.resize(256);
        memcpy(_chunk_indices.data(), data.data(), 256 * sizeof(decltype(_chunk_indices)::value_type));
        return true;
    }

    adt_tile::adt_tile(const uint32_t x, const uint32_t y, const utils::binary_reader_ptr &reader) : _x(x), _y(y) {
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
    }
}
