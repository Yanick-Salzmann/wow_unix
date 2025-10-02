#include "texture.h"
#include <stb_image.h>

namespace wow::gl {
    static GLuint default_texture = 0;

    texture::texture() {
        _texture = default_texture;
    }

    texture::~texture() {
        if (_texture != default_texture) {
            glDeleteTextures(1, &_texture);
        }
    }

    void texture::bind() {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    void texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture::rgba_image(uint32_t width, uint32_t height, const void *data) {
        if (_texture == default_texture) {
            glGenTextures(1, &_texture);
        }

        bind();
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        unbind();
    }

    void texture::bgra_image(uint32_t width, uint32_t height, const void *data) {
        if (_texture == default_texture) {
            glGenTextures(1, &_texture);
        }

        bind();
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_BGRA,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            0,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        unbind();
    }

    void texture::image(const uint32_t width, const uint32_t height, const GLint format, const void *data) {
        if (_texture == default_texture) {
            glGenTextures(1, &_texture);
        }
        bind();
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);
        unbind();
    }

    void texture::filtering(GLint min_filter, GLint mag_filter) {
        if (_texture == default_texture) {
            return;
        }

        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        unbind();
    }

    void texture::wrap(GLint wrap_s, GLint wrap_t) {
        if (_texture == default_texture) {
            return;
        }

        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
        unbind();
    }

    struct stb_deleter {
        void operator()(void *ptr) const {
            stbi_image_free(ptr);
        }
    };

    void texture::png_image(const std::string &path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        const auto data = std::unique_ptr<stbi_uc, stb_deleter>(
            stbi_load(path.c_str(), &width, &height, &channels, 4)
        );
        rgba_image(width, height, data.get());
    }

    GLuint texture::native() const {
        return _texture == default_texture ? 0 : _texture;
    }

    void texture::initialize_default_texture() {
        glGenTextures(1, &default_texture);
        glBindTexture(GL_TEXTURE_2D, default_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     static_cast<const GLvoid *>("\xff\x00\xff\xff\x00\x00\x00\xff\x00\x00\x00\xff\xff\x00\xff\xff")
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
