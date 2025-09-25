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

        template<typename T>
        binary_reader &read(std::vector<T> &data) {
            read(data.data(), data.size() * sizeof(T));
            return *this;
        }
    };

    using binary_reader_ptr = std::shared_ptr<binary_reader>;

    std::vector<uint8_t> to_png(const std::vector<uint8_t>& data, uint32_t w, uint32_t h);
}

#endif //WOW_UNIX_IO_H
