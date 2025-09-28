#include "camera.h"

#include "gl/mesh.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace wow::scene {
    camera::camera(gl::window_ptr window) : _window(std::move(window)) {
        _view = glm::lookAt(
            _position,
            _forward,
            _up
        );

        _projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        _matrix_changed = true;
    }

    void camera::update_aspect_ratio(const float aspect) {
        _projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        _matrix_changed = true;
    }

    void camera::update() {
        if (!_is_in_world) {
            return;
        }

        auto diff = std::chrono::steady_clock::now() - _last_update;
        _last_update = std::chrono::steady_clock::now();
        const auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / 1000.0f;
        const float camera_speed = 5.0f;

        if (_window->is_key_pressed(GLFW_KEY_W)) {
            _position += _forward * camera_speed * delta_time;
            _updated = true;
        } else if (_window->is_key_pressed(GLFW_KEY_S)) {
            _position -= _forward * camera_speed * delta_time;
            _updated = true;
        } else if (_window->is_key_pressed(GLFW_KEY_A)) {
        }


        if (_updated) {
            _view = glm::lookAt(
                _position,
                _position + _forward,
                _up
            );

            _updated = false;
            _matrix_changed = true;
        }

        if (_matrix_changed) {
            const auto mesh = gl::mesh::terrain_mesh();
            const auto view_projection = _projection * _view;
            mesh->program()->use();
            mesh->program()->mat4(view_projection, "view_projection");
        }
    }

    void camera::enter_world(const glm::vec3 &position) {
        _last_update = std::chrono::steady_clock::now();
        _is_in_world = true;
        _position = position;
        _updated = true;
    }
}
