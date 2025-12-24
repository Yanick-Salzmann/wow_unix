#include "blp_file.h"
#include <stdexcept>

namespace wow::io::blp {
    void blp_file::load_format() {
        _format = blp_format::unknown;

        switch (_header.compression) {
            case 1:
                _format = blp_format::rgb_palette;
                break;

            case 2: {
                switch (_header.alpha_compression) {
                    case 0:
                        _format = blp_format::bc1;
                        break;
                    case 1:
                        _format = blp_format::bc2;
                        break;
                    case 7:
                        _format = blp_format::bc3;
                        break;
                    default:
                        break;
                }
                break;
            }

            case 3:
                _format = blp_format::rgb;
                break;

            default:
                break;
        }
    }

    void rgb565_to_rgb8_array(const uint32_t &input, uint8_t *output) {
        auto r = input & 0x1Fu;
        auto g = input >> 5u & 0x3Fu;
        auto b = input >> 11u & 0x1Fu;

        r = (r << 3u) | (r >> 2u);
        g = (g << 2u) | (g >> 4u);
        b = (b << 3u) | (b >> 2u);

        output[0] = static_cast<uint8_t>(b);
        output[1] = static_cast<uint8_t>(g);
        output[2] = static_cast<uint8_t>(r);
    }

    // ReSharper disable once CppDFAUnreachableFunctionCall
    void blp_file::read_bc_colors(utils::binary_reader &reader, std::array<rgb_data_array, 4> &colors,
                                  const bool pre_multiplied_alpha,
                                  const bool use_4_colors) {
        const auto color1 = reader.read<uint16_t>();
        const auto color2 = reader.read<uint16_t>();

        rgb565_to_rgb8_array(color1, colors[0].data.buffer);
        rgb565_to_rgb8_array(color2, colors[1].data.buffer);

        for (auto i = 0; i < 4; ++i) {
            colors[i].data.buffer[3] = 0xFF;
        }

        if (use_4_colors || color1 > color2) {
            for (auto i = 0; i < 3; ++i) {
                colors[3].data.buffer[i] = static_cast<uint8_t>(
                    (colors[0].data.buffer[i] + 2 * colors[1].data.buffer[i]) / 3);
                colors[2].data.buffer[i] = static_cast<uint8_t>(
                    (2 * colors[0].data.buffer[i] + colors[1].data.buffer[i]) / 3);
            }
        } else {
            for (auto i = 0; i < 3; ++i) {
                colors[3].data.buffer[i] = 0;
                colors[2].data.buffer[i] = static_cast<uint8_t>(
                    (colors[0].data.buffer[i] + colors[1].data.buffer[i]) / 2);
            }

            if (pre_multiplied_alpha) {
                colors[3].data.buffer[3] = 0;
            }
        }
    }

    void blp_file::process_block_bc1(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                     const size_t block_offset) {
        std::array<rgb_data_array, 4> colors{};
        read_bc_colors(reader, colors, true, false);
        const auto indices = reader.read<uint32_t>();
        for (auto i = 0; i < 16; ++i) {
            const auto idx = static_cast<uint8_t>((indices >> (i * 2)) & 0x3);
            block_data[block_offset + i] = colors[idx].data.color;
        }
    }

    void blp_file::process_block_bc2(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                     const size_t block_offset) {
        uint8_t alpha_values[16]{};
        const auto alpha = reader.read<uint64_t>();
        for (auto i = 0; i < 16; ++i) {
            alpha_values[i] = static_cast<uint8_t>((alpha >> (i * 4)) & 0xF) * 17;
        }

        std::array<rgb_data_array, 4> colors{};
        read_bc_colors(reader, colors, false, true);

        const auto indices = reader.read<uint32_t>();
        for (auto i = 0; i < 16; ++i) {
            const auto idx = static_cast<uint8_t>((indices >> (i * 2)) & 0x3);
            block_data[block_offset + i] = (colors[idx].data.color & 0x00FFFFFF) | (alpha_values[i] << 24);
        }
    }

    void blp_file::process_block_bc3(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                     const size_t block_offset) {
        uint8_t alpha_values[8]{};
        uint8_t alpha_lookup[16]{};

        const auto alpha1 = static_cast<uint32_t>(reader.read<uint8_t>());
        const auto alpha2 = static_cast<uint32_t>(reader.read<uint8_t>());

        alpha_values[0] = static_cast<uint8_t>(alpha1);
        alpha_values[1] = static_cast<uint8_t>(alpha2);

        if (alpha1 > alpha2) {
            for (auto i = 0; i < 6; ++i) {
                alpha_values[i + 2] = static_cast<uint8_t>(((6 - i) * alpha1 + (1 + i) * alpha2) / 7);
            }
        } else {
            for (auto i = 0; i < 4; ++i) {
                alpha_values[i + 2] = static_cast<uint8_t>(((4 - i) * alpha1 + (1 + i) * alpha2) / 5);
            }

            alpha_values[6] = 0;
            alpha_values[7] = 0xFF;
        }

        uint64_t lookup_value{};
        reader.read(&lookup_value, 6);
        for (auto i = 0; i < 16; ++i) {
            alpha_lookup[i] = static_cast<uint8_t>((lookup_value >> (i * 3)) & 0x7);
        }

        std::array<rgb_data_array, 4> colors{};
        read_bc_colors(reader, colors, false, true);

        const auto indices = reader.read<uint32_t>();
        for (auto i = 0; i < 16; ++i) {
            const auto idx = static_cast<uint8_t>((indices >> (i * 2)) & 0x3);
            block_data[block_offset + i] = colors[idx].data.color |
                                           (alpha_values[alpha_lookup[i]] << 24);
        }
    }

    void blp_file::process_palette_fast_path(std::vector<uint8_t> &rgba_data,
                                             const uint32_t w, const uint32_t h,
                                             const std::vector<uint8_t> &layer_data) const {
        std::vector<uint32_t> row_buffer(w);

        const auto num_entries = w * h;
        auto counter = 0;

        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w; ++x) {
                const auto index = layer_data[counter];
                const auto alpha = static_cast<uint32_t>(layer_data[num_entries + counter]);
                const auto color = (_palette[index] & 0x00FFFFFF) | (alpha << 24);
                row_buffer[x] = color;

                ++counter;
            }

            memcpy(rgba_data.data() + y * w * 4, row_buffer.data(), w * 4);
        }
    }

    void blp_file::process_palette_full_path(std::vector<uint8_t> &rgba_data,
                                             const uint32_t w, const uint32_t h,
                                             const std::vector<uint8_t> &layer_data) const {
        const auto num_entries = w * h;

        std::vector<uint32_t> color_buffer(num_entries);

        for (auto i = 0; i < num_entries; ++i) {
            const auto index = layer_data[i];
            const auto color = _palette[index];
            color_buffer[i] = color | 0xFF000000;
        }

        switch (_header.alpha_depth) {
            case 0:
                break;

            case 1: {
                auto counter = 0;
                for (auto i = 0; i < (num_entries / 8) + (num_entries % 8 ? 1 : 0); ++i) {
                    const auto alpha_byte = layer_data[num_entries + i];
                    for (auto bit = 0; bit < (i == (num_entries / 8) ? (num_entries % 8) : 8); ++bit) {
                        const auto alpha = (alpha_byte & (1 << bit)) ? 0xFF : 0x00;
                        color_buffer[counter] = (color_buffer[counter] & 0x00FFFFFF) | (alpha << 24);
                        ++counter;
                    }
                }
                break;
            }

            case 4: {
                static uint32_t ALPHA_LOOKUP4[] = {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB,
                    0xCC, 0xDD, 0xEE, 0xFF
                };

                auto counter = 0;
                for (auto i = 0; i < (num_entries / 2) + (num_entries % 2 ? 1 : 0); ++i) {
                    const auto alpha_byte = layer_data[num_entries + i];
                    const auto alpha1 = ALPHA_LOOKUP4[alpha_byte & 0x0F];
                    color_buffer[counter++] |= alpha1 << 24;
                    if (i != num_entries / 2) {
                        const auto alpha2 = ALPHA_LOOKUP4[alpha_byte >> 4];
                        color_buffer[counter++] |= alpha2 << 24;
                    }
                }
            }

            default:
                throw std::runtime_error("Unsupported alpha depth");
        }

        memcpy(rgba_data.data(), color_buffer.data(), num_entries * 4);
    }

    void blp_file::unwrap_compressed_blp_layer(
        const uint32_t w, const uint32_t h,
        std::vector<uint8_t> &rgba_data,
        const std::vector<uint8_t> &layer_data) const {
        const auto converter = block_converter_function();

        const auto num_blocks = ((w + 3) / 4) * ((h + 3) / 4);
        std::vector<uint32_t> block_data(num_blocks * 16);

        utils::binary_reader reader{layer_data};
        for (auto i = 0; i < num_blocks; ++i) {
            converter(reader, block_data, i * 16);
        }

        std::vector<uint32_t> color_buffer(w * h);
        auto counter = 0;
        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w; ++x) {
                const auto bx = x / 4;
                const auto by = y / 4;

                const auto ibx = x % 4;
                const auto iby = y % 4;

                const auto block_index = by * ((w + 3) / 4) + bx;
                const auto inner_index = iby * 4 + ibx;
                const auto color = block_data[block_index * 16 + inner_index];
                color_buffer[counter++] = color;
            }
        }

        memcpy(rgba_data.data(), color_buffer.data(), w * h * 4);
    }

    void blp_file::unwrap_blp_layer_with_palette(std::vector<uint8_t> &rgba_data,
                                                 const uint32_t w, const uint32_t h,
                                                 const std::vector<uint8_t> &layer_data) const {
        if (_header.alpha_depth == 0) {
            process_palette_fast_path(rgba_data, w, h, layer_data);
        } else {
            process_palette_full_path(rgba_data, w, h, layer_data);
        }
    }

    void blp_file::unwrap_blp_layer(std::vector<uint8_t> &rgba_data, const uint32_t w, const uint32_t h,
                                    const uint32_t layer) const {
        const auto layer_data = _mipmaps[layer];
        switch (_format) {
            case blp_format::rgb: {
                if (rgba_data.size() != layer_data.size()) {
                    throw std::runtime_error("Invalid BLP layer size");
                }

                memcpy(rgba_data.data(), layer_data.data(), layer_data.size());
                break;
            }

            case blp_format::rgb_palette: {
                unwrap_blp_layer_with_palette(rgba_data, w, h, layer_data);
                break;
            }

            case blp_format::bc1:
            case blp_format::bc2:
            case blp_format::bc3: {
                unwrap_compressed_blp_layer(w, h, rgba_data, layer_data);
                break;
            }


            default: {
                throw std::runtime_error("Unsupported BLP format");
            }
        }
    }

    blp_file::blp_file(const mpq_file_ptr &file) {
        if (!file) {
            throw std::runtime_error("Invalid MPQ file pointer");
        }

        file->read(_header);
        load_format();

        if (_header.magic != '2PLB') {
            throw std::runtime_error("Invalid BLP file format");
        }

        if (_format == blp_format::rgb_palette) {
            _palette.resize(256);
            file->read(_palette);
        }

        uint32_t mipmap_count = 0;

        for (uint32_t i = 0; i < 16; ++i) {
            if (_header.mipmap_sizes[i] > 0) {
                mipmap_count = i + 1;
            } else {
                break;
            }
        }

        _mipmaps.resize(mipmap_count);

        for (uint32_t i = 0; i < mipmap_count; ++i) {
            if (_header.mipmap_sizes[i] > 0) {
                file->seek(_header.mipmap_offsets[i]);
                _mipmaps[i].resize(_header.mipmap_sizes[i]);
                file->read(_mipmaps[i]);
            }
        }
    }

    std::vector<uint8_t> blp_file::palette_layer_to_rgba(const uint32_t layer) const {
        if (_format != blp_format::rgb_palette) {
            throw std::runtime_error("Invalid BLP format: Layer is not paletted");
        }

        std::vector<uint8_t> rgba_data{};
        this->unwrap_blp_layer_with_palette(rgba_data, std::max(1u, _header.width >> layer),
                                            std::max(1u, _header.height >> layer), _mipmaps[layer]);

        return rgba_data;
    }

    std::vector<uint8_t> blp_file::convert_to_rgba() const {
        if (_format == blp_format::unknown) {
            throw std::runtime_error("Unsupported BLP format for conversion");
        }

        std::vector<uint8_t> rgba_data;
        rgba_data.resize(_header.width * _header.height * 4);
        unwrap_blp_layer(rgba_data, _header.width, _header.height, 0);
        return rgba_data;
    }

    std::vector<uint8_t> blp_file::convert_to_rgba(uint32_t dimension, uint32_t &w, uint32_t &h) const {
        if (_format == blp_format::unknown) {
            throw std::runtime_error("Unsupported BLP format for conversion");
        }

        auto layer = 0;
        if (dimension < _header.width) {
            bool found = false;
            for (int i = 1; i < _mipmaps.size(); ++i) {
                if (const auto current_size = _header.width >> i;
                    current_size <= dimension) {
                    found = true;
                    layer = i;
                    break;
                }
            }

            if (!found) {
                layer = _mipmaps.size() - 1;
            }
        }

        dimension = _header.width >> layer;
        w = dimension;
        h = dimension;

        std::vector<uint8_t> rgba_data;
        rgba_data.resize(dimension * dimension * 4);
        unwrap_blp_layer(rgba_data, dimension, dimension, layer);
        return rgba_data;
    }

    void write_to_vector(void *context, void *data, const int size) {
        const auto v = static_cast<std::vector<uint8_t> *>(context);
        v->insert(v->end(), static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + size);
    }

    std::vector<uint8_t> blp_file::convert_to_png() const {
        const auto rgba_data = convert_to_rgba();
        return utils::to_png(rgba_data, _header.width, _header.height);
    }
}
