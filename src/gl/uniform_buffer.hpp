#ifndef WOW_UNIX_UNIFORM_BUFFER_HPP
#define WOW_UNIX_UNIFORM_BUFFER_HPP

#include <array>
#include <glad/gl.h>

#include <memory>
#include <vector>

namespace wow::gl {
    class uniform_buffer {
        GLuint _buffer{};

        void update_data(const void *data, size_t size, size_t offset = 0) const;

    public:
        uniform_buffer();

        template<typename T>
        void update_data(const T &data, const size_t offset = 0) {
            update_data(&data, sizeof(T), offset);
        }

        template<typename T, size_t size>
        void update_data(const std::array<T, size> &data, const size_t offset = 0) {
            update_data(data.data(), size * sizeof(T), offset);
        }

        template<typename T>
        void update_data(const std::vector<T> &data, const size_t offset = 0) {
            update_data(data.data(), data.size() * sizeof(T), offset);
        }

        void bind() const;
    };

    typedef std::shared_ptr<uniform_buffer> uniform_buffer_ptr;

    inline uniform_buffer_ptr make_uniform_buffer() {
        return std::make_shared<uniform_buffer>();
    }
}

#endif //WOW_UNIX_UNIFORM_BUFFER_HPP
