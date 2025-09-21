#ifndef WOW_UNIX_MPQ_FILE_H
#define WOW_UNIX_MPQ_FILE_H

#include <memory>
#include <StormLib.h>
#include <vector>

namespace wow::io {
    class mpq_file {
        HANDLE _file{};
        std::vector<uint8_t> _buffer{};
        size_t _offset{};

    public:
        explicit mpq_file(HANDLE file);

        [[nodiscard]] size_t size() const {
            return _buffer.size();
        }

        [[nodiscard]] size_t position() const {
            return _offset;
        }

        mpq_file &seek(size_t offset) {
            _offset = offset;
            return *this;
        }

        mpq_file &seek_rel(ptrdiff_t diff) {
            if (diff < 0 && abs(diff) > _offset) {
                _offset = 0;
            } else {
                _offset += diff;
            }

            return *this;
        }

        void read(size_t size, void *buffer);

        template<typename T>
        void read(T &value) {
            read(sizeof(T), &value);
        }

        template<typename T>
        T read() {
            T value;
            read(value);
            return value;
        }

        template<typename T>
        mpq_file &read(std::vector<T> &data) {
            read(data.size() * sizeof(T), data.data());
            return *this;
        }

        template<typename T, size_t Size>
        mpq_file &read(T (&data)[Size]) {
            read(Size * sizeof(T), data);
            return *this;
        }

        std::string read_text();
    };

    typedef std::shared_ptr<mpq_file> mpq_file_ptr;
}

#endif //WOW_UNIX_MPQ_FILE_H
