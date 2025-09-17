#include "shared_texture.h"

#include <vector>
#include <drm/drm_fourcc.h>
#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_EGL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <mutex>

#include "spdlog/spdlog.h"

namespace wow::gl {
    typedef void (GLAPIENTRY*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES{};

    std::once_flag egl_load_flag{};

    uint32_t drm_fourcc_from_cef(const cef_color_type_t format) {
        switch (format) {
            case CEF_COLOR_TYPE_BGRA_8888:
                return DRM_FORMAT_BGRA8888;
            case CEF_COLOR_TYPE_RGBA_8888:
                return DRM_FORMAT_RGBA8888;
            default:
                return 0;
        }
    }

    shared_texture::shared_texture(
        const cef_color_type_t type,
        uint32_t width,
        uint32_t height,
        const std::vector<cef_accelerated_paint_native_pixmap_plane_t> &planes
    ) {
        std::call_once(egl_load_flag, [] {
            glEGLImageTargetTexture2DOES = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
                eglGetProcAddress(
                    "glEGLImageTargetTexture2DOES"
                )
            );
        });

        const auto fourcc = drm_fourcc_from_cef(type);
        std::vector<EGLAttrib> attribute_list{};
        attribute_list.insert(attribute_list.end(), {
                                  EGL_WIDTH, width,
                                  EGL_HEIGHT, height,
                                  EGL_LINUX_DRM_FOURCC_EXT, fourcc
                              });

        if (!planes.empty()) {
            attribute_list.insert(
                attribute_list.end(), {
                    EGL_DMA_BUF_PLANE0_OFFSET_EXT, static_cast<EGLAttrib>(planes[0].fd),
                    EGL_DMA_BUF_PLANE0_PITCH_EXT, static_cast<EGLAttrib>(planes[0].stride),
                    EGL_DMA_BUF_PLANE0_OFFSET_EXT, static_cast<EGLAttrib>(planes[0].offset)
                }
            );
        }

        if (planes.size() > 1) {
            attribute_list.insert(
                attribute_list.end(), {
                    EGL_DMA_BUF_PLANE1_OFFSET_EXT, static_cast<EGLAttrib>(planes[1].fd),
                    EGL_DMA_BUF_PLANE1_PITCH_EXT, static_cast<EGLAttrib>(planes[1].stride),
                    EGL_DMA_BUF_PLANE1_OFFSET_EXT, static_cast<EGLAttrib>(planes[1].offset)
                }
            );
        }

        if (planes.size() > 2) {
            attribute_list.insert(
                attribute_list.end(), {
                    EGL_DMA_BUF_PLANE2_OFFSET_EXT, static_cast<EGLAttrib>(planes[2].fd),
                    EGL_DMA_BUF_PLANE2_PITCH_EXT, static_cast<EGLAttrib>(planes[2].stride),
                    EGL_DMA_BUF_PLANE2_OFFSET_EXT, static_cast<EGLAttrib>(planes[2].offset)
                }
            );
        }

        if (planes.size() > 3) {
            attribute_list.insert(
                attribute_list.end(), {
                    EGL_DMA_BUF_PLANE3_OFFSET_EXT, static_cast<EGLAttrib>(planes[3].fd),
                    EGL_DMA_BUF_PLANE3_PITCH_EXT, static_cast<EGLAttrib>(planes[3].stride),
                    EGL_DMA_BUF_PLANE3_OFFSET_EXT, static_cast<EGLAttrib>(planes[3].offset)
                }
            );
        }

        if (planes.size() > 4) {
            SPDLOG_ERROR("Too many planes in shared texture: {}", planes.size());
            throw std::runtime_error("Too many planes in shared texture");
        }

        _image = eglCreateImage(glfwGetEGLDisplay(), nullptr, EGL_LINUX_DMA_BUF_EXT, nullptr,
                                attribute_list.data());

        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, _image);
    }

    shared_texture::~shared_texture() {
        eglDestroyImage(glfwGetEGLDisplay(), _image);
        glDeleteTextures(1, &_texture);
    }

    void shared_texture::bind() {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }
}
