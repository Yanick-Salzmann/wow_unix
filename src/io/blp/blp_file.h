#ifndef WOW_UNIX_BLP_FILE_H
#define WOW_UNIX_BLP_FILE_H

#include "io/mpq_file.h"
#include "utils/io.h"
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

namespace wow::io::blp {
    enum class blp_format {
        rgb,
        rgb_palette,
        bc1,
        bc2,
        bc3,
        unknown
    };

    struct blp_header {
        uint32_t magic;
        uint32_t version;
        uint8_t compression;
        uint8_t alpha_depth;
        uint8_t alpha_compression;
        uint8_t has_mipmaps;
        uint32_t width;
        uint32_t height;
        uint32_t mipmap_offsets[16];
        uint32_t mipmap_sizes[16];
    };

    class blp_file {
        struct rgb_data_array {
            rgb_data_array() {
                data.color = 0;
            }

            union data {
                uint32_t color;
                uint8_t buffer[4];
            } data{};
        };

        blp_header _header{};
        std::vector<uint32_t> _palette;
        std::vector<std::vector<uint8_t> > _mipmaps;
        blp_format _format = blp_format::unknown;

        void load_format();

        typedef std::function<void(utils::binary_reader &, std::vector<uint32_t> &, size_t)> block_converter_function_t;

        [[nodiscard]] block_converter_function_t block_converter_function() const {
            switch (_format) {
                case blp_format::bc1:
                    return &blp_file::process_block_bc1;
                case blp_format::bc2:
                    return &blp_file::process_block_bc2;
                case blp_format::bc3:
                    return &blp_file::process_block_bc3;
                default:
                    return {};
            }
        }

        static void read_bc_colors(utils::binary_reader &reader, std::array<rgb_data_array, 4> &colors,
                                   bool pre_multiplied_alpha,
                                   bool use_4_colors);

        static void process_block_bc1(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                      size_t block_offset);

        static void process_block_bc2(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                      size_t block_offset);

        static void process_block_bc3(utils::binary_reader &reader, std::vector<uint32_t> &block_data,
                                      size_t block_offset);

        void process_palette_fast_path(std::vector<uint8_t> &rgba_data, uint32_t w, uint32_t h,
                                       const std::vector<uint8_t> &layer_data) const;

        void process_palette_full_path(std::vector<uint8_t> &rgba_data, uint32_t w, uint32_t h,
                                       const std::vector<uint8_t> &layer_data) const;

        void unwrap_compressed_blp_layer(uint32_t w, uint32_t h, std::vector<uint8_t> &rgba_data,
                                         const std::vector<uint8_t> &layer_data) const;

        void unwrap_blp_layer_with_palette(std::vector<uint8_t> &rgba_data,
                                           uint32_t w, uint32_t h,
                                           const std::vector<uint8_t> &layer_data) const;

        void unwrap_blp_layer(std::vector<uint8_t> &rgba_data, uint32_t w, uint32_t h, uint32_t layer) const;

    public:
        explicit blp_file(const mpq_file_ptr &file);

        [[nodiscard]] uint32_t width() const {
            return _header.width;
        }

        [[nodiscard]] uint32_t height() const {
            return _header.height;
        }

        [[nodiscard]] blp_format format() const {
            return _format;
        }

        [[nodiscard]] std::vector<uint8_t> convert_to_rgba() const;

        [[nodiscard]] std::vector<uint8_t> convert_to_rgba(uint32_t dimension, uint32_t& w, uint32_t& h) const;

        [[nodiscard]] std::vector<uint8_t> convert_to_png() const;
    };

    using blp_file_ptr = std::shared_ptr<blp_file>;
}

#endif //WOW_UNIX_BLP_FILE_H
