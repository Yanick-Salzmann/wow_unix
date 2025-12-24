#ifndef WOW_UNIX_VERTEX_BUFFER_H
#define WOW_UNIX_VERTEX_BUFFER_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace wow::gl {
    class vertex_buffer {
        GLuint _buffer{};

    public:
        vertex_buffer();

        ~vertex_buffer();

        void bind() const;

        void unbind();

        void set_data(const void *data, size_t size);

        template<typename T>
        void set_data(const std::vector<T> &data) {
            set_data(data.data(), data.size() * sizeof(T));
        }

        template<typename T, size_t size>
        void set_data(const T (&data)[size]) {
            set_data(data, size * sizeof(T));
        }

        template<typename T, size_t size>
        void set_data(const std::array<T, size> &data) {
            set_data(data.data(), size * sizeof(T));
        }
    };

    using vertex_buffer_ptr = std::shared_ptr<vertex_buffer>;

    inline vertex_buffer_ptr make_vertex_buffer() {
        return std::make_shared<vertex_buffer>();
    }
}

#endif //WOW_UNIX_VERTEX_BUFFER_H
