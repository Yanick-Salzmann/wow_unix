#include "camera.h"

#include "utils/di.h"

#include "gl/mesh.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "spdlog/spdlog.h"

namespace wow::scene {
    camera::camera(gl::window_ptr window) : _window(std::move(window)) {
        _view = glm::lookAtLH(
            _position,
            _forward,
            _up
        );

        _projection = glm::perspectiveLH(glm::radians(45.0f), 1.0f, 0.1f, 2000.0f);
        _matrix_changed = true;
    }

    void camera::update_aspect_ratio(const float aspect) {
        _projection = glm::perspectiveLH(glm::radians(45.0f), aspect, 0.1f, 2000.0f);
        _matrix_changed = true;
    }

    bool camera::update() {
        if (!_is_in_world) {
            return false;
        }

        const auto diff = std::chrono::steady_clock::now() - _last_update;
        _last_update = std::chrono::steady_clock::now();
        const auto delta_time = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count())
                                / 1000.0f;

        constexpr float camera_speed = 100.0f;

        if (_window->is_key_pressed(GLFW_KEY_W)) {
            _position += _forward * camera_speed * delta_time;
            _updated = true;
        }

        if (_window->is_key_pressed(GLFW_KEY_S)) {
            _position -= _forward * camera_speed * delta_time;
            _updated = true;
        }

        if (_window->is_key_pressed(GLFW_KEY_Q)) {
            _position -= glm::vec3{0, 0, 1} * camera_speed * delta_time;
            _updated = true;
        }

        if (_window->is_key_pressed(GLFW_KEY_E)) {
            _position += glm::vec3{0, 0, 1} * camera_speed * delta_time;
            _updated = true;
        }

        if (_window->is_key_pressed(GLFW_KEY_A)) {
            _position -= _right * camera_speed * delta_time;
            _updated = true;
        }

        if (_window->is_key_pressed(GLFW_KEY_D)) {
            _position += _right * camera_speed * delta_time;
            _updated = true;
        }

        const auto mouse_pos = utils::app_module->window()->get_mouse_position();

        if (_window->is_mouse_button_pressed(GLFW_MOUSE_BUTTON_2)) {
            const auto mouse_diff = mouse_pos - _last_mouse_pos;
            auto dx = static_cast<float>(mouse_diff.x);
            auto dy = static_cast<float>(mouse_diff.y);

            if (dx != 0.0f || dy != 0.0f) {
                constexpr float sensitivity = 0.1f;
                dx *= sensitivity;
                dy *= sensitivity;

                const auto rotation_x = glm::angleAxis(glm::radians(-dx), glm::vec3(0.0f, 0.0f, 1.0f));
                _forward = _forward * rotation_x;
                _right = _right * rotation_x;
                _forward = glm::normalize(_forward);
                _right = glm::normalize(_right);

                const auto rotation_y = glm::angleAxis(glm::radians(-dy), _right);
                _forward = _forward * rotation_y;
                _forward = glm::normalize(_forward);
                _up = glm::cross(_forward, _right);
                _up = glm::normalize(_up);

                _updated = true;
            }
        }

        _last_mouse_pos = mouse_pos;

        const auto result = _updated;
        if (_updated) {
            _view = glm::lookAtLH(
                _position,
                _position + _forward,
                _up
            );

            _updated = false;
            _matrix_changed = true;
        }

        if (_matrix_changed) {
            _frustum.update_view_projection(_projection * _view);
            const auto mesh = gl::mesh::terrain_mesh();
            mesh->program()->use();
            mesh->program()->mat4(_view, "view");
            mesh->program()->mat4(_projection, "projection");
            _matrix_changed = false;
        }

        return result;
    }

    void camera::enter_world(const glm::vec3 &position) {
        _last_update = std::chrono::steady_clock::now();
        _is_in_world = true;
        _position = position;
        _updated = true;
    }

    void camera::update_position(const glm::vec3 &position) {
        _position = position;
        _updated = true;
    }
}
