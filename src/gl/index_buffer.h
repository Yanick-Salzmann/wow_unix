#ifndef WOW_UNIX_INDEX_BUFFER_H
#define WOW_UNIX_INDEX_BUFFER_H

#include <memory>
#include <vector>
#include <glad/glad.h>

namespace wow::gl {
    class index_buffer {
        GLuint _buffer{};
        GLenum _type{};

    public:
        explicit index_buffer(GLenum type);

        ~index_buffer();

        index_buffer &bind();

        index_buffer &unbind();

        index_buffer &set_data(const void *data, size_t size);

        template<typename T>
        index_buffer &set_data(const std::vector<T> &data) {
            set_data(data.data(), data.size() * sizeof(T));
            return *this;
        }

        template<typename T, size_t size>
        index_buffer &set_data(const T (&data)[size]) {
            set_data(data, size * sizeof(T));
            return *this;
        }

        template<typename T, size_t size>
        index_buffer &set_data(const std::array<T, size> &data) {
            set_data(data.data(), size * sizeof(T));
            return *this;
        }

        [[nodiscard]] const GLenum &type() const {
            return _type;
        }
    };

    using index_buffer_ptr = std::shared_ptr<index_buffer>;
}

#endif //WOW_UNIX_INDEX_BUFFER_H
