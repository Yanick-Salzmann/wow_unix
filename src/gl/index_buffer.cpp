#include "index_buffer.h"

namespace wow::gl {
    index_buffer::index_buffer(index_type type) {
        _type = static_cast<GLenum>(type);
        glGenBuffers(1, &_buffer);
    }

    index_buffer::~index_buffer() {
        glDeleteBuffers(1, &_buffer);
    }

    index_buffer &index_buffer::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
        return *this;
    }

    index_buffer &index_buffer::unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return *this;
    }

    index_buffer &index_buffer::set_data(const void *data, size_t size) {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
        unbind();
        return *this;
    }
}
