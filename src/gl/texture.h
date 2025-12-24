#ifndef WOW_UNIX_TEXTURE_H
#define WOW_UNIX_TEXTURE_H

#include <memory>
#include <glad/gl.h>

#include "bindable_texture.h"
#include "io/blp/blp_file.h"

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

        void image(uint32_t width, uint32_t height, GLint format, const void *data);

        void load_blp(const io::blp::blp_file_ptr &blp);

        void filtering(GLint min_filter, GLint mag_filter);
        void wrap(GLint wrap_s, GLint wrap_t);

        void png_image(const std::string &path);

        GLuint native() const;

        static void initialize_default_texture();
    };

    using texture_ptr = std::shared_ptr<texture>;

    inline texture_ptr make_texture() {
        return std::make_shared<texture>();
    }
}

#endif //WOW_UNIX_TEXTURE_H
