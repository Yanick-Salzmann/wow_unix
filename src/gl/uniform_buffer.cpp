#include "uniform_buffer.hpp"

namespace wow::gl {
    void uniform_buffer::update_data(const void *data, const size_t size, const size_t offset) const {
        bind();
        glBindBuffer(GL_UNIFORM_BUFFER, _buffer);
        if (offset > 0) {
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        } else {
            glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
        }
    }

    uniform_buffer::uniform_buffer() {
        glGenBuffers(1, &_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _buffer);
    }

    void uniform_buffer::bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, _buffer);
    }
}
