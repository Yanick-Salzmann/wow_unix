#include "adt_chunk.h"

#include "spdlog/spdlog.h"
#include "utils/constants.h"
#include "utils/di.h"

#include "adt_tile.h"

namespace wow::io::terrain {
    gl::index_buffer_ptr adt_chunk::_index_buffer;
    uint32_t adt_chunk::_alpha_uniform{};
    uint32_t adt_chunk::_color_uniforms[4]{};

    void adt_chunk::load_alpha_rle(const uint32_t layer, const utils::binary_reader_ptr &reader) {
        auto num_read = 0;
        while (num_read < 4096) {
            const auto indicator = reader->read<uint8_t>();
            const auto repeat = indicator & 0x7F;
            if ((indicator & 0x80) != 0) {
                const auto value = static_cast<uint32_t>(reader->read<uint8_t>());
                for (auto i = 0; i < repeat; ++i) {
                    _texture_data[num_read++] |= static_cast<uint32_t>(value) << (layer * 8);
                }
            } else {
                for (auto i = 0; i < repeat; ++i) {
                    _texture_data[num_read++] |= static_cast<uint32_t>(reader->read<uint8_t>()) << (layer * 8);
                }
            }
        }
    }

    void adt_chunk::load_alpha_uncompressed(const uint32_t layer, const utils::binary_reader_ptr &reader) {
        for (auto i = 0; i < 4096; ++i) {
            _texture_data[i] |= static_cast<uint32_t>(reader->read<uint8_t>()) << (layer * 8);
        }
    }

    void adt_chunk::load_alpha_compressed(const uint32_t layer, const utils::binary_reader_ptr &reader) {
        auto out_ptr = _texture_data.data();

        const auto full_alpha = _header.flags.unfixed_alpha_map != 0;
        for (auto k = 0; k < (full_alpha ? 64 : 63); ++k) {
            for (auto j = 0; j < 32; ++j) {
                const auto val = reader->read<uint8_t>();
                auto val1 = static_cast<uint8_t>(val & 0x0F);
                auto val2 = static_cast<uint8_t>((j == 31 && !full_alpha) ? val1 : (val >> 4));

                val1 = static_cast<uint8_t>(val1 * 17);
                val2 = static_cast<uint8_t>(val2 * 17);

                *out_ptr++ |= static_cast<uint32_t>(val1) << (layer * 8);
                *out_ptr++ |= static_cast<uint32_t>(val2) << (layer * 8);
            }
        }

        if (!full_alpha) {
            for (auto j = 0; j < 64; ++j) {
                _texture_data[63 * 64 + j] |= ((_texture_data[62 * 64 + j] >> (layer * 8)) & 0xFF) << (layer * 8);
            }
        }
    }

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

                const auto ax = x / 8.0f + ((y % 2 != 0) ? 0.5f / 8.0f : 0.0f);
                const auto ay = y / 16.0f;
                const auto tx = x + (y % 2 != 0 ? 0.5f : 0.0f);
                const auto ty = y * 0.5f;

                vec.tex_coord = glm::vec2(tx, ty);
                vec.alpha_coord = glm::vec2(ax, ay);

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

    void adt_chunk::load_shadows(const utils::binary_reader_ptr &reader) {
        if (_texture_data.empty()) {
            _texture_data.assign(4096, 0x000000000);
        }

        for (auto i = 0; i < 4096; ++i) {
            _texture_data[i] &= 0x00FFFFFF;
        }

        if (!reader) {
            return;
        }

        if (reader->read<uint32_t>() != 'MCSH') {
            SPDLOG_ERROR("Chunk has invalid MCSH chunk, signature mismatch");
            return;
        }

        if (reader->read<uint32_t>() < (4096 / 8) * sizeof(uint8_t)) {
            SPDLOG_ERROR("Chunk has invalid MCSH chunk, size too small");
            return;
        }

        for (auto i = 0; i < 64; ++i) {
            auto val = reader->read<uint64_t>();
            for (auto j = 0; j < 64; ++j) {
                _texture_data[i * 64 + j] |= (((val & 0x1) ? 0xFF : 0x00) << 24);
                val >>= 1;
            }
        }
    }

    void adt_chunk::load_layers(const utils::binary_reader_ptr &reader) {
        if (_header.num_layers < 1) {
            return;
        }

        if (reader->read<uint32_t>() != 'MCLY') {
            SPDLOG_ERROR("Chunk has invalid MCLY chunk, signature mismatch");
            return;
        }

        if (reader->read<uint32_t>() < _header.num_layers * sizeof(mcly)) {
            SPDLOG_ERROR("Chunk has invalid MCLY chunk, size too small");
            return;
        }

        _layers.resize(_header.num_layers);
        reader->read(_layers);

        for (auto i = 0; i < _header.num_layers; ++i) {
            const auto layer = _layers[i];
            auto tex = _parent_tile.lock()->find_texture(layer.texture_id);
            if (!tex) {
                SPDLOG_ERROR("Chunk has invalid MCLY chunk, texture not found");
                return;
            }

            _textures.push_back(tex);
        }
    }

    void adt_chunk::load_alpha(const utils::binary_reader_ptr &reader) {
        if (reader->read<uint32_t>() != 'MCAL') {
            SPDLOG_ERROR("Chunk has invalid MCAL chunk, signature mismatch");
            return;
        }

        _texture_data.assign(4096, 0x000000000);

        for (auto i = 1; i < _header.num_layers; ++i) {
            const auto layer = _layers[i];
            if (!layer.flags.use_alpha_map && !layer.flags.alpha_map_compressed) {
                for (auto k = 0; k < 4096; ++k) {
                    _texture_data[k] |= (0xFF << ((i - 1) * 8));
                }
                continue;
            }

            if (layer.offset_mcal + _header.ofs_alpha + 8 >= reader->size()) {
                SPDLOG_WARN("Invalid MCAL chunk, offset too large");
                continue;
            }

            reader->seek(_header.ofs_alpha + layer.offset_mcal + 8);

            if (layer.flags.alpha_map_compressed) {
                load_alpha_rle(i - 1, reader);
            } else if (_use_big_alpha) {
                load_alpha_uncompressed(i - 1, reader);
            } else {
                load_alpha_compressed(i - 1, reader);
            }

            reader->seek(layer.offset_mcal);
        }
    }

    void adt_chunk::sync_load() {
        static std::once_flag flag{};
        std::call_once(flag, [] {
            _index_buffer = std::make_shared<gl::index_buffer>(gl::index_type::uint16);

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
            gl::mesh::terrain_mesh().mesh->index_buffer(_index_buffer);


            _alpha_uniform = gl::mesh::terrain_mesh().mesh->program()->uniform_location("shadow_texture");
            for (auto i = 0; i < 4; ++i) {
                _color_uniforms[i] = gl::mesh::terrain_mesh().mesh->program()->uniform_location(
                    fmt::format("color_texture{}", i));
            }
        });

        _shadow_texture = gl::make_texture();
        _shadow_texture->bgra_image(64, 64, _texture_data.data());
        _shadow_texture->filtering(GL_LINEAR, GL_LINEAR);
        _shadow_texture->wrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        _texture_data.clear();


        _is_sync_loaded = true;
        utils::app_module->map_manager()->add_load_progress();
    }

    adt_chunk::adt_chunk(
        const wdt_file_ptr &wdt,
        const adt_tile_ptr &tile,
        const utils::binary_reader_ptr &reader
    ) : _parent_tile(tile) {
        if (reader->read<uint32_t>() != 'MCNK') {
            SPDLOG_ERROR("Chunk has invalid MCNK chunk, signature mismatch");
            return;
        }

        reader->seek_mod(4);
        _header = reader->read<map_chunk_header>();

        if (!_header.ofs_heights) {
            return;
        }

        _use_big_alpha = wdt->has_large_alpha();

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

        _parent_tile.lock()->update_vectors(_vectors, (_header.index_y * 16 + _header.index_x) * 145);

        if (_header.num_layers > 0) {
            reader->seek(_header.ofs_layer);
            load_layers(reader);
        }

        if (_header.num_layers > 1 && _header.size_alpha > 8 && _header.ofs_alpha > 0) {
            reader->seek(_header.ofs_alpha);
            load_alpha(reader);
        }

        if (_header.size_shadow > 8) {
            reader->seek(_header.ofs_shadow);
            load_shadows(reader);
        } else {
            load_shadows(nullptr);
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

    void adt_chunk::on_frame(const scene::scene_info &scene_info) {
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

        if (!_bounds.intersects_sphere(scene_info.camera_position, scene_info.view_distance)) {
            return;
        }

        if (!utils::app_module->camera()->view_frustum().intersects_aabb(_bounds)) {
            return;
        }

        const auto mesh = gl::mesh::terrain_mesh().mesh;
        for (auto i = 0; i < _header.num_layers; ++i) {
            mesh->texture(_color_uniforms[i], _textures[i]);
        }

        mesh->texture(_alpha_uniform, _shadow_texture);
        mesh->bind_textures();

        mesh->draw(true, (_header.index_y * 16 + _header.index_x) * 145);
    }

    uint32_t vector_index(const uint32_t row, const uint32_t column) {
        const uint32_t prev_rows = 17u * (row / 2u) + ((row % 2u) ? 9u : 0u);
        return prev_rows + column;
    }

    float adt_chunk::height(float x, const float y) const {
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
