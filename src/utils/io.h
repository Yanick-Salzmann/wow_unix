#ifndef WOW_UNIX_IO_H
#define WOW_UNIX_IO_H

#include <vector>
#include <cstdint>
#include <memory>

namespace wow::utils {
    class binary_reader {
        std::vector<uint8_t> _data{};
        size_t _offset = 0;

    public:
        explicit binary_reader(std::vector<uint8_t> data);

        void read(void *data, size_t size);

        binary_reader &seek_mod(const ssize_t diff) {
            _offset += diff;
            return *this;
        }

        binary_reader &seek(const size_t offset) {
            _offset = offset;
            return *this;
        }

        template<typename T>
        T read() {
            T val{};
            read(&val, sizeof(T));
            return val;
        }

        template<typename T>
        binary_reader &read(T &value) {
            value = read<T>();
            return *this;
        }

        template<typename T, size_t size>
        binary_reader &read(T (&data)[size]) {
            read(data, size * sizeof(T));
            return *this;
        }

        template<typename T, size_t size>
        binary_reader &read(std::array<T, size> &data) {
            read(data.data(), size * sizeof(T));
            return *this;
        }

        template<typename T>
        binary_reader &read(std::vector<T> &data) {
            read(data.data(), data.size() * sizeof(T));
            return *this;
        }

        [[nodiscard]] bool eof() const {
            return _offset >= _data.size();
        }

        [[nodiscard]] std::size_t size() const {
            return _data.size();
        }
    };

    using binary_reader_ptr = std::shared_ptr<binary_reader>;

    std::vector<uint8_t> to_png(const std::vector<uint8_t> &data, uint32_t w, uint32_t h);

    inline binary_reader_ptr make_binary_reader(const std::vector<uint8_t> &data) {
        return std::make_shared<binary_reader>(data);
    }

    std::vector<uint8_t> read_png_image_to_bitmap(const std::string& path, size_t& width, size_t& height);
}

#endif //WOW_UNIX_IO_H
