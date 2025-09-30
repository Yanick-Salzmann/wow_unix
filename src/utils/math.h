#ifndef WOW_UNIX_MATH_H
#define WOW_UNIX_MATH_H
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/geometric.hpp"

namespace wow::utils {
    class bounding_box {
        glm::vec3 _min{}, _max{};

    public:
        bounding_box() = default;

        bounding_box(const glm::vec3 min, const glm::vec3 max) : _min(min), _max(max) {
        }

        [[nodiscard]] glm::vec3 min() const {
            return _min;
        }

        glm::vec3 &min() {
            return _min;
        }

        [[nodiscard]] glm::vec3 max() const {
            return _max;
        }

        glm::vec3 &max() {
            return _max;
        }

        bounding_box &take_min(const glm::vec3 p) {
            _min.x = std::min(p.x, _min.x);
            _min.y = std::min(p.y, _min.y);
            _min.z = std::min(p.z, _min.z);

            return *this;
        }

        bounding_box &take_max(const glm::vec3 p) {
            _max.x = std::max(p.x, _max.x);
            _max.y = std::max(p.y, _max.y);
            _max.z = std::max(p.z, _max.z);

            return *this;
        }

        bounding_box &take_min_max(const glm::vec3 p) {
            return take_min(p).take_max(p);
        }
    };

    class plane {
        glm::vec3 _normal{0.0f, 1.0f, 0.0f};
        float _distance{0.0f};

    public:
        plane() = default;

        plane(const glm::vec3 &normal, const float distance)
            : _normal(glm::normalize(normal)), _distance(distance) {
        }

        plane(const glm::vec3 &normal, const glm::vec3 &point)
            : _normal(glm::normalize(normal)), _distance(glm::dot(_normal, point)) {
        }

        plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
            _normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            _distance = glm::dot(_normal, p1);
        }

        explicit plane(const glm::vec4 &equation)
            : _normal(equation.x, equation.y, equation.z), _distance(equation.w) {
            const auto length = glm::length(_normal);
            _normal /= length;
            _distance /= length;
        }

        [[nodiscard]] const glm::vec3 &normal() const { return _normal; }

        glm::vec3 &normal() {
            return _normal;
        }

        [[nodiscard]] float distance() const { return _distance; }

        float &distance() {
            return _distance;
        }

        [[nodiscard]] glm::vec4 equation() const {
            return {_normal, _distance};
        }

        [[nodiscard]] float distance_to_point(const glm::vec3 &point) const {
            return glm::dot(_normal, point) - _distance;
        }

        [[nodiscard]] glm::vec3 closest_point(const glm::vec3 &point) const {
            return point - _normal * distance_to_point(point);
        }

        [[nodiscard]] bool is_point_above(const glm::vec3 &point, const float epsilon = 1e-6f) const {
            return distance_to_point(point) > epsilon;
        }

        [[nodiscard]] bool is_point_below(const glm::vec3 &point, const float epsilon = 1e-6f) const {
            return distance_to_point(point) < -epsilon;
        }

        [[nodiscard]] bool is_point_on(const glm::vec3 &point, const float epsilon = 1e-6f) const {
            return std::abs(distance_to_point(point)) <= epsilon;
        }

        [[nodiscard]] bool intersect_ray(const glm::vec3 &origin, const glm::vec3 &direction,
                                         float &t, const float epsilon = 1e-6f) const {
            const auto dot_normal = glm::dot(_normal, direction);
            if (std::abs(dot_normal) < epsilon) {
                return false;
            }

            t = (_distance - glm::dot(_normal, origin)) / dot_normal;
            return t >= 0.0f;
        }

        [[nodiscard]] bool intersect_line(const glm::vec3 &origin, const glm::vec3 &direction,
                                          float &t, const float epsilon = 1e-6f) const {
            const auto dot_normal = glm::dot(_normal, direction);
            if (std::abs(dot_normal) < epsilon) {
                return false;
            }

            t = (_distance - glm::dot(_normal, origin)) / dot_normal;
            return true;
        }

        plane &flip() {
            _normal = -_normal;
            _distance = -_distance;
            return *this;
        }

        [[nodiscard]] plane flipped() const {
            return {-_normal, -_distance};
        }
    };
}

#endif //WOW_UNIX_MATH_H
