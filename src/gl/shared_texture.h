#ifndef WOW_UNIX_SHARED_TEXTURE_H
#define WOW_UNIX_SHARED_TEXTURE_H

#ifndef _WIN32
#include <vector>
#include <EGL/egl.h>

#include "glad/gl.h"
#include "include/internal/cef_types_color.h"
#include "include/internal/cef_types_linux.h"

#include "bindable_texture.h"

namespace wow::gl {
    class shared_texture final : public bindable_texture {
        EGLImage _image{};
        GLuint _texture{};

    public:
        shared_texture(
            cef_color_type_t type,
            uint32_t width,
            uint32_t height,
            const std::vector<cef_accelerated_paint_native_pixmap_plane_t> &planes
        );

        ~shared_texture() override;
        void bind() override;
        void unbind() const;

    };
}

#endif

#endif //WOW_UNIX_SHARED_TEXTURE_H
