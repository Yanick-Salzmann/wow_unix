#include <glad/gl.h>
#ifndef _WIN32
#include <execinfo.h>
#endif
#include "window.h"

#include "mesh.h"
#include "texture.h"
#include "spdlog/spdlog.h"
#include "utils/di.h"

namespace wow::gl {
    void gl_debug_callback(const GLenum source, GLenum type, GLuint id, const GLenum severity, GLsizei length,
                           const GLchar *message, const void *userParam) {
        const std::string source_name = (source == GL_DEBUG_SOURCE_API
                                             ? "API"
                                             : source == GL_DEBUG_SOURCE_WINDOW_SYSTEM
                                                   ? "WINDOW SYSTEM"
                                                   : source == GL_DEBUG_SOURCE_SHADER_COMPILER
                                                         ? "SHADER COMPILER"
                                                         : source == GL_DEBUG_SOURCE_THIRD_PARTY
                                                               ? "THIRD PARTY"
                                                               : source == GL_DEBUG_SOURCE_APPLICATION
                                                                     ? "APPLICATION"
                                                                     : "UNKNOWN");

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                SPDLOG_ERROR("GL CALLBACK: {} type = 0x{} id = 0x{} message = {}", source_name, type, id, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                SPDLOG_WARN("GL CALLBACK: {} type = 0x{} id = 0x{} message = {}", source_name, type, id, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                SPDLOG_INFO("GL CALLBACK: {} type = 0x{} id = 0x{} message = {}", source_name, type, id, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                SPDLOG_DEBUG("GL CALLBACK: {} type = 0x{} id = 0x{} message = {}", source_name, type, id, message);
                break;
            default:
                SPDLOG_INFO("GL CALLBACK: {} type = 0x{} id = 0x{} message = {}", source_name, type, id, message);
                break;
        }
    }

    void window::report_glfw_error(const std::string &msg) {
        const char *error{};
        glfwGetError(&error);
        SPDLOG_ERROR("Error in GLFW call: {}, error: {}", msg, error);
    }

    void window::on_resize(int width, int height) const {
        glfwGetFramebufferSize(_window, &width, &height);
        glViewport(0, 0, width, height);
        utils::app_module->camera()->update_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
    }

    window::window() {
        if (!glfwInit()) {
            report_glfw_error("Cannot initialize GLFW");
            throw std::runtime_error("Cannot initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _window = glfwCreateWindow(1024, 768, "wow-unix", nullptr, nullptr);
        if (!_window) {
            report_glfw_error("Cannot create GLFW window");
            throw std::runtime_error("Cannot create GLFW window");
        }

        size_t icon_w{}, icon_h{};
        auto data = utils::read_png_image_to_bitmap("app_icon.png", icon_w, icon_h);
        GLFWimage icon_image{};
        icon_image.width = static_cast<int32_t>(icon_w);
        icon_image.height = static_cast<int32_t>(icon_h);
        icon_image.pixels = data.data();

        glfwSetWindowIcon(_window, 1, &icon_image);

        glfwMakeContextCurrent(_window);
        glfwSwapInterval(1);
        glfwSetWindowUserPointer(_window, this);

        glfwSetWindowSizeCallback(_window, [](GLFWwindow *window, const int width, const int height) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            wnd->on_resize(width, height);
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_resize_callbacks) {
                cb(width, height);
            }
        });

        glfwSetMouseButtonCallback(_window, [](GLFWwindow *window, const int button, const int action, const int mods) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_mouse_button_callbacks) {
                cb(button, action, mods);
            }
        });

        glfwSetKeyCallback(_window, [](GLFWwindow *window, const int key, const int scancode, const int action,
                                       const int mods) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_key_callbacks) {
                cb(key, scancode, action, mods);
            }
        });

        glfwSetScrollCallback(_window, [](GLFWwindow *window, const double x_offset, const double y_offset) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_scroll_callbacks) {
                cb(x_offset, y_offset);
            }
        });

        glfwSetCursorPosCallback(_window, [](GLFWwindow *window, const double x_pos, const double y_pos) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_mouse_move_callbacks) {
                cb(x_pos, y_pos);
            }
        });

        glfwSetCharCallback(_window, [](GLFWwindow *window, const unsigned int codepoint) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_char_callbacks) {
                cb(codepoint);
            }
        });

        glfwSetWindowFocusCallback(_window, [](GLFWwindow *window, const int focused) {
            const auto wnd = static_cast<gl::window *>(glfwGetWindowUserPointer(window));
            std::lock_guard lock{wnd->_callback_lock};
            for (const auto &cb: wnd->_focus_callbacks) {
                cb(focused == GLFW_TRUE);
            }
        });

        if (!gladLoadGL(glfwGetProcAddress)) {
            SPDLOG_ERROR("Cannot initialize GLAD");
            throw std::runtime_error("Cannot initialize GLAD");
        }

        texture::initialize_default_texture();
        mesh::terrain_mesh();

        glDebugMessageCallback(gl_debug_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        glClearColor(1.0f, 0.5f, 0.25f, 1.0f);

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        SPDLOG_INFO("Window size: {}x{}", width, height);

        SPDLOG_INFO("OpenGL version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
        SPDLOG_INFO("OpenGL vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
        SPDLOG_INFO("GLSL version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

        glViewport(0, 0, width, height);
    }

    window::~window() {
        if (_window) {
            terminate();
        }
    }

    bool window::process_events() const {
        glfwPollEvents();
        return glfwWindowShouldClose(_window) == 0;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void window::begin_frame() {
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void window::end_frame() const {
        glfwSwapBuffers(_window);
    }

    void window::terminate() {
        glfwDestroyWindow(_window);
        glfwTerminate();
        _window = nullptr;
    }

    std::pair<int, int> window::size() const {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        return {width, height};
    }

    void window::change_cursor(const cef_cursor_type_t cursor) const {
        switch (cursor) {
            case CT_POINTER:
                glfwSetCursor(_window, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
                break;
            case CT_CROSS:
                glfwSetCursor(_window, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
                break;
            case CT_HAND:
                glfwSetCursor(_window, glfwCreateStandardCursor(GLFW_HAND_CURSOR));
                break;
            case CT_IBEAM:
                glfwSetCursor(_window, glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
                break;
            default:
                break;
        }
    }

    std::pair<int, int> window::screen_coordinates(const int x, const int y) const {
        int win_x, win_y;
        glfwGetWindowPos(_window, &win_x, &win_y);

        int frame_x, frame_y, ignored;
        glfwGetWindowFrameSize(_window, &frame_x, &frame_y, &ignored, &ignored);

        return {x + win_x + frame_x, y + win_y + frame_y};
    }

    bool window::is_key_pressed(const int key) const {
        return glfwGetKey(_window, key) == GLFW_PRESS;
    }

    bool window::is_mouse_button_pressed(const int button) const {
        return glfwGetMouseButton(_window, button) == GLFW_PRESS;
    }
}
