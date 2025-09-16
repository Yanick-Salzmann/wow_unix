#ifndef WOW_UNIX_TEXTURE_H
#define WOW_UNIX_TEXTURE_H

#include <memory>
#include <glad/glad.h>

#include "bindable_texture.h"

namespace wow::gl {
    class texture : public bindable_texture {
        GLuint _texture{};

    public:
        texture();

        ~texture() override;

        void bind() override;

        void unbind();

        void rgba_image(uint32_t width, uint32_t height, const void *data);
        void bgra_image(uint32_t width, uint32_t height, const void *data);

        void png_image(const std::string &path);
    };

    typedef std::shared_ptr<texture> texture_ptr;

    inline texture_ptr make_texture() {
        return std::make_shared<texture>();
    }
}

#endif //WOW_UNIX_TEXTURE_H
