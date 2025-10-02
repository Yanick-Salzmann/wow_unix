#include "adt_chunk.h"

#include "spdlog/spdlog.h"
#include "utils/constants.h"
#include "utils/di.h"

namespace wow::io::terrain {
    gl::index_buffer_ptr adt_chunk::_index_buffer;

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

    void adt_chunk::load_normals(const utils::binary_reader_ptr &reader) {
        if (reader->read<uint32_t>() != 'MCNR') {
            SPDLOG_ERROR("Chunk has invalid MCNR chunk, signature mismatch");
            return;
        }

        if (reader->read<uint32_t>() < 145 * 3 * sizeof(int8_t)) {
            SPDLOG_ERROR("Chunk has invalid MCNR chunk, size too small");
            return;
        }

        std::array<int8_t, 145 * 3> normals{};
        reader->read(normals);
        for (auto i = 0; i < 145; ++i) {
            auto &vec = _vectors[i];
            vec.normal.x = static_cast<float>(normals[i * 3]) / -127.0f;
            vec.normal.y = static_cast<float>(normals[i * 3 + 1]) / -127.0f;
            vec.normal.z = static_cast<float>(normals[i * 3 + 2]) / 127.0f;
        }
    }

    void adt_chunk::load_colors(const utils::binary_reader_ptr &reader) {
        for (auto &vec: _vectors) {
            vec.vertex_color = glm::vec3(0.5f, 0.5f, 0.5f);
        }

        if (reader) {
            if (reader->read<uint32_t>() != 'MCCV') {
                SPDLOG_ERROR("Chunk has invalid MCCV chunk, signature mismatch");
                return;
            }

            if (reader->read<uint32_t>() < 145 * 4 * sizeof(uint8_t)) {
                SPDLOG_ERROR("Chunk has invalid MCCV chunk, size too small");
                return;
            }

            std::array<glm::i8vec4, 145> colors{};
            reader->read(colors);
            for (auto i = 0; i < 145; ++i) {
                auto &vec = _vectors[i];
                vec.vertex_color = glm::vec3(colors[i].r / 255.0f, colors[i].g / 255.0f, colors[i].b / 255.0f);
            }
        }
    }

    void adt_chunk::sync_load() {
        static std::once_flag flag{};
        std::call_once(flag, [] {
            _index_buffer = std::make_shared<gl::index_buffer>(GL_UNSIGNED_SHORT);

            std::array<uint16_t, 768> indices{};
            for (auto y = 0u; y < 8; ++y) {
                for (auto x = 0u; x < 8; ++x) {
                    const auto i = y * 8 * 12 + x * 12;
                    indices[i + 0] = y * 17 + x;
                    indices[i + 2] = y * 17 + x + 9;
                    indices[i + 1] = y * 17 + x + 1;

                    indices[i + 3] = y * 17 + x + 1;
                    indices[i + 5] = y * 17 + x + 9;
                    indices[i + 4] = y * 17 + x + 18;

                    indices[i + 6] = y * 17 + x + 18;
                    indices[i + 8] = y * 17 + x + 9;
                    indices[i + 7] = y * 17 + x + 17;

                    indices[i + 9] = y * 17 + x + 17;
                    indices[i + 11] = y * 17 + x + 9;
                    indices[i + 10] = y * 17 + x;
                }
            }

            _index_buffer->set_data(indices);
        });

        _vertex_buffer = std::make_shared<gl::vertex_buffer>();
        _vertex_buffer->set_data(_vectors);

        _is_sync_loaded = true;
        utils::app_module->map_manager()->add_load_progress();
    }

    adt_chunk::adt_chunk(const utils::binary_reader_ptr &reader) {
        if (reader->read<uint32_t>() != 'MCNK') {
            SPDLOG_ERROR("Chunk has invalid MCNK chunk, signature mismatch");
            return;
        }

        reader->seek_mod(4); // MCNK size
        _header = reader->read<map_chunk_header>();

        if (!_header.ofs_heights) {
            return;
        }

        reader->seek(_header.ofs_heights);
        load_heights(reader);
        reader->seek(_header.ofs_normals);
        load_normals(reader);

        if (_header.ofs_mccv > 0) {
            reader->seek(_header.ofs_mccv);
            load_colors(reader);
        } else {
            load_colors(nullptr);
        }

        _bounds = utils::bounding_box(_vectors[0].position, _vectors[0].position);
        for (const auto &v: _vectors) {
            _bounds.take_min_max(v.position);
        }

        if (_bounds.max().z - _bounds.min().z < 5) {
            _bounds.max().z = _bounds.min().z + 5;
        }

        _is_async_loaded = true;
    }

    void adt_chunk::on_frame() {
        if (!_is_async_loaded) {
            return;
        }

        if (!_is_sync_loaded) {
            if (_sync_load_requested) {
                return;
            }

            _sync_load_requested = true;
            utils::app_module->gpu_dispatcher()->dispatch([this] { sync_load(); });
            return;
        }

        if (!utils::app_module->camera()->view_frustum().intersects_aabb(_bounds)) {
            return;
        }

        const auto mesh = gl::mesh::terrain_mesh();
        mesh->vertex_buffer(_vertex_buffer)
                .index_buffer(_index_buffer);

        mesh->draw();
    }

    uint32_t vector_index(const uint32_t row, const uint32_t column) {
        auto index = 0;
        for (auto y = 0; y < row; ++y) {
            index += (y % 2) ? 8 : 9;
        }

        index += column;
        return index;
    }

    float adt_chunk::height(float x, float y) const {
        const auto row = static_cast<int32_t>(y / 17);
        x -= std::max(0.0f, (row % 2) ? (0.5f * utils::VERTEX_SIZE) : 0.0f);
        const auto column = static_cast<int32_t>(x / utils::VERTEX_SIZE);

        const auto index = vector_index(row, column);
        if (index >= _vectors.size()) {
            return -std::numeric_limits<float>::infinity();
        }

        return _vectors[index].position.z;
    }
}
