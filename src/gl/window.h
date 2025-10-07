#ifndef WOW_UNIX_WINDOW_H
#define WOW_UNIX_WINDOW_H

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"

#define GLFW_EXPOSE_NATIVE_X11
#include "GLFW/glfw3native.h"
#include "include/internal/cef_types.h"

namespace wow::gl {
    class window {
    public:
        using mouse_move_callback = std::function<void(double x, double y)>;
        using mouse_button_callback = std::function<void(int button, int action, int mods)>;
        using scroll_callback = std::function<void(double x_offset, double y_offset)>;
        using key_callback = std::function<void(int key, int scancode, int action, int mods)>;
        using char_callback = std::function<void(unsigned int codepoint)>;
        using resize_callback = std::function<void(int width, int height)>;

    private:
        GLFWwindow *_window{};

        std::mutex _callback_lock{};
        std::vector<mouse_move_callback> _mouse_move_callbacks{};
        std::vector<mouse_button_callback> _mouse_button_callbacks{};
        std::vector<scroll_callback> _scroll_callbacks{};
        std::vector<key_callback> _key_callbacks{};
        std::vector<char_callback> _char_callbacks{};
        std::vector<resize_callback> _resize_callbacks{};


        static void report_glfw_error(const std::string &msg);

        void on_resize(int width, int height) const;

    public:
        window();

        void add_mouse_move_callback(mouse_move_callback cb) {
            std::lock_guard lock(_callback_lock);
            _mouse_move_callbacks.push_back(std::move(cb));
        }

        void add_mouse_button_callback(mouse_button_callback cb) {
            std::lock_guard lock(_callback_lock);
            _mouse_button_callbacks.push_back(std::move(cb));
        }

        void add_scroll_callback(scroll_callback cb) {
            std::lock_guard lock(_callback_lock);
            _scroll_callbacks.push_back(std::move(cb));
        }

        void add_key_callback(key_callback cb) {
            std::lock_guard lock(_callback_lock);
            _key_callbacks.push_back(std::move(cb));
        }

        void add_char_callback(char_callback cb) {
            std::lock_guard lock(_callback_lock);
            _char_callbacks.push_back(std::move(cb));
        }

        void add_resize_callback(resize_callback cb) {
            std::lock_guard lock(_callback_lock);
            _resize_callbacks.push_back(std::move(cb));
        }

        ~window();

        [[nodiscard]] bool process_events() const;

        void begin_frame();

        void end_frame() const;

        void terminate();

        [[nodiscard]] std::pair<int, int> size() const;

        [[nodiscard]] float dpi_scaling() const {
            float x_scale = 1.0f, y_scale = 1.0f;
            if (_window) {
                glfwGetWindowContentScale(_window, &x_scale, &y_scale);
            }
            return x_scale;
        }

        [[nodiscard]] XID handle() const {
            return glfwGetX11Window(_window);
        }

        [[nodiscard]] GLFWwindow *native_handle() const {
            return _window;
        }

        void change_cursor(cef_cursor_type_t cursor) const;

        [[nodiscard]] std::pair<int, int> screen_coordinates(int x, int y) const;

        [[nodiscard]] bool is_key_pressed(int key) const;

        [[nodiscard]] bool is_mouse_button_pressed(int button) const;

        [[nodiscard]] glm::vec2 get_mouse_position() const {
            double x, y;
            glfwGetCursorPos(_window, &x, &y);
            return {x, y};
        }
    };

    using window_ptr = std::shared_ptr<window>;

    inline window_ptr make_window() {
        return std::make_shared<window>();
    }
}


#undef Status

#endif //WOW_UNIX_WINDOW_H
