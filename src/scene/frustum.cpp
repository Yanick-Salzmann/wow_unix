#include "frustum.h"
#include <algorithm>

#include "glm/gtc/type_ptr.hpp"

namespace wow::scene {
    frustum::frustum(const glm::mat4 &view, const glm::mat4 &projection) {
        update(view, projection);
    }

    void frustum::update(const glm::mat4 &view, const glm::mat4 &projection) {
        const auto view_projection = view * projection;
        extract_planes(view_projection);
    }

    void frustum::update_view_projection(const glm::mat4 &view_projection) {
        extract_planes(view_projection);
    }

    void frustum::extract_planes(const glm::mat4 &view_projection) {
        const auto m = glm::transpose(view_projection);

        _planes[LEFT] = utils::plane(m[3] + m[0]);
        _planes[RIGHT] = utils::plane(m[3] - m[0]);
        _planes[BOTTOM] = utils::plane(m[3] + m[1]);
        _planes[TOP] = utils::plane(m[3] - m[1]);
        _planes[NEAR] = utils::plane(m[3] + m[2]);
        _planes[FAR] = utils::plane(m[3] - m[2]);
    }

    bool intersects(const utils::plane &plane, const glm::vec3 &min_point, const glm::vec3 &max_point) {
        const auto x = (plane.normal().x > 0) ? max_point.x : min_point.x;
        const auto y = (plane.normal().y > 0) ? max_point.y : min_point.y;
        const auto z = (plane.normal().z > 0) ? max_point.z : min_point.z;

        const auto dp = plane.normal().x * x + plane.normal().y * y + plane.normal().z * z;
        return dp >= -plane.distance();
    }

    bool frustum::intersects_aabb(const glm::vec3 &min_point, const glm::vec3 &max_point) const {
        for (auto plane: _planes) {
            if (!intersects(plane, min_point, max_point)) {
                return false;
            }
        }

        return true;
    }

    bool frustum::contains_point(const glm::vec3 &point) const {
        return std::ranges::all_of(_planes, [&point](const auto &plane) {
            return plane.distance_to_point(point) >= 0;
        });
    }
}
