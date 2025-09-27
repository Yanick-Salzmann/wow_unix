#include "adt_chunk.h"

#include "spdlog/spdlog.h"
#include "utils/constants.h"

namespace wow::io::terrain {
    void adt_chunk::load_heights(const utils::binary_reader_ptr &reader) {
        if (reader->read<uint32_t>() != 'MCVT') {
            SPDLOG_ERROR("Chunk has invalid MCVT chunk, signature mismatch");
            return;
        }

        if (reader->read<uint32_t>() < 145 * sizeof(float)) {
            SPDLOG_ERROR("Chunk has invalid MCVT chunk, size too small");
            return;
        }

        std::array<float, 145> heights{};
        reader->read(heights);

        auto counter = 0;

        const auto base_x = utils::MAP_MID_POINT - _header.position.y;
        const auto base_y = utils::MAP_MID_POINT - _header.position.x;

        for (int y = 0; y < 17; ++y) {
            const auto columns = (y % 2) ? 8 : 9;
            for (int x = 0; x < columns; ++x) {
                auto &vec = _vectors[counter];
                vec.position.z = _header.position.z + heights[counter];
                vec.position.x = base_x + static_cast<float>(x) * utils::VERTEX_SIZE;
                vec.position.y = base_y + static_cast<float>(y) * utils::VERTEX_SIZE / 2.0f;

                if (columns == 8) {
                    vec.position.x += utils::VERTEX_SIZE / 2.0f;
                }

                ++counter;
            }
        }
    }

    adt_chunk::adt_chunk(const utils::binary_reader_ptr &reader) {
        if (auto signature = reader->read<uint32_t>(); signature != 'MCNK') {
            SPDLOG_ERROR("Invalid chunk signature, got {:08X}", signature);
            return;
        }

        reader->seek_mod(4); // MCNK size
        _header = reader->read<map_chunk_header>();

        if (!_header.ofs_heights) {
            return;
        }

        reader->seek(_header.ofs_heights);
        load_heights(reader);
    }
}
