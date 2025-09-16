#ifndef WOW_UNIX_WEB_CORE_H
#define WOW_UNIX_WEB_CORE_H

#include "web_application.h"
#include "web_client.h"
#include <future>
#include <thread>

#include "gl/mesh.h"
#include "gl/texture.h"
#include "gl/window.h"

namespace wow::web {
    class web_client;

    class web_core : public std::enable_shared_from_this<web_core> {
        friend class web_client;

        CefRefPtr<web_application> _application{};
        CefRefPtr<web_client> _client{};

        std::packaged_task<bool()> _task{};
        std::thread _message_loop{};

        gl::window_ptr _window{};

        gl::mesh_ptr _mesh{};
        gl::texture_ptr _texture{};
        std::vector<uint8_t> _image_data{};
        int32_t _width = 0, _height = 0;

        double _mouse_x = 0, _mouse_y = 0;
        double _last_click_x = 0, _last_click_y = 0;
        long _last_click_time{};
        int _last_click_button = 0;
        int _last_click_count = 0;

        bool _is_dirty = false;
        bool _has_loaded = false;

        std::mutex _image_lock{};

        void on_paint(int32_t width, int32_t height, const void *data);
        void update_texture();

        int calculate_modifiers() const;

    public:
        explicit web_core(gl::window_ptr  window);

        void initialize(int argc, char *argv[]);
        void shutdown();

        [[nodiscard]] gl::texture_ptr texture() const {
            return _texture;
        }

        void render();
    };

    using web_core_ptr = std::shared_ptr<web_core>;

    inline web_core_ptr make_web_core(gl::window_ptr window) {
        return std::make_shared<web_core>(std::move(window));
    }
}

#endif //WOW_UNIX_WEB_CORE_H