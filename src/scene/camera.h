#ifndef WOW_UNIX_CAMERA_H
#define WOW_UNIX_CAMERA_H

#include <chrono>
#include <memory>
#include <glm/mat4x4.hpp>

#include "gl/window.h"

namespace wow::scene {
    class camera {
        glm::mat4 _view{};
        glm::mat4 _projection{};

        glm::vec3 _position{};
        glm::vec3 _forward{1.0f, 0.0f, 0.0f};
        glm::vec3 _up{0.0f, 0.0f, 1.0f};

        bool _is_in_world = false;
        bool _updated = false;
        bool _matrix_changed = false;

        gl::window_ptr _window{};

        std::chrono::steady_clock::time_point _last_update{};

    public:
        explicit camera(gl::window_ptr window);

        void update_aspect_ratio(float aspect);

        [[nodiscard]] const glm::mat4 &view() const {
            return _view;
        }

        [[nodiscard]] const glm::mat4 &projection() const {
            return _projection;
        }

        void update();

        void enter_world(const glm::vec3& position);

        void leave_world() {
            _is_in_world = false;
        }
    };

    using camera_ptr = std::shared_ptr<camera>;
}

#endif //WOW_UNIX_CAMERA_H
