#include <glad/gl.h>

#include "vertex_buffer.h"

namespace wow::gl {
    vertex_buffer::vertex_buffer() {
        glGenBuffers(1, &_buffer);
    }

    vertex_buffer::~vertex_buffer() {
        glDeleteBuffers(1, &_buffer);
    }

    void vertex_buffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    }

    void vertex_buffer::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void vertex_buffer::set_data(const void *data, size_t size) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
        unbind();
    }
}
