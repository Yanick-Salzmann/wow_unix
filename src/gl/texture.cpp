#include "texture.h"
#include <stb_image.h>

namespace wow::gl {
    texture::texture() {
        glGenTextures(1, &_texture);
    }

    texture::~texture() {
        glDeleteTextures(1, &_texture);
    }

    void texture::bind() {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    void texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture::rgba_image(uint32_t width, uint32_t height, const void *data) {
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
}
