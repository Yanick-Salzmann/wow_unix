#include "io.h"

#include <cstring>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdexcept>

namespace wow::utils {
    binary_reader::binary_reader(std::vector<uint8_t> data) : _data(std::move(data)) {
    }

    void binary_reader::read(void *data, const size_t size) {
        if (_offset + size > _data.size()) {
            throw std::runtime_error("binary reader read out of bounds");
        }

        memcpy(data, _data.data() + _offset, size);
        _offset += size;
    }

    void write_to_vector(void *context, void *data, const int size) {
        const auto v = static_cast<std::vector<uint8_t> *>(context);
        v->insert(v->end(), static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + size);
    }

    std::vector<uint8_t> to_png(const std::vector<uint8_t> &data, const uint32_t w, const uint32_t h) {
        std::vector<uint8_t> png_data{};
        stbi_write_png_to_func(write_to_vector, &png_data, static_cast<int>(w), static_cast<int>(h), 4,
                               data.data(), static_cast<int>(w) * 4);
        return png_data;
    }

    std::vector<uint8_t> read_png_image_to_bitmap(const std::string &path, size_t &width, size_t &height) {
        int32_t w, h, c;
        const auto data = stbi_load(path.c_str(), &w, &h, &c, 4);
        if (!data) {
            throw std::runtime_error("failed to load image");
        }

        width = w;
        height = h;
        auto ret = std::vector(data, data + w * h * 4);
        stbi_image_free(data);
        return ret;
    }
}
