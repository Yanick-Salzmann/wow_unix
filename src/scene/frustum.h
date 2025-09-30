#ifndef WOW_UNIX_FRUSTUM_H
#define WOW_UNIX_FRUSTUM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/math.h"

namespace wow::scene {
    class frustum {
        utils::plane _planes[6];

        void extract_planes(const glm::mat4 &view_projection);

    public:
        enum plane_index {
            LEFT = 0,
            RIGHT = 1,
            TOP = 2,
            BOTTOM = 3,
            NEAR = 4,
            FAR = 5
        };

        frustum() = default;

        frustum(const glm::mat4 &view, const glm::mat4 &projection);

        void update(const glm::mat4 &view, const glm::mat4 &projection);

        void update_view_projection(const glm::mat4 &view_projection);

        [[nodiscard]] bool intersects_aabb(const glm::vec3 &min_point, const glm::vec3 &max_point) const;

        [[nodiscard]] bool intersects_aabb(const utils::bounding_box &box) const {
            return intersects_aabb(box.min(), box.max());
        }

        [[nodiscard]] bool contains_point(const glm::vec3 &point) const;

        [[nodiscard]] const utils::plane &get_plane(const plane_index index) const { return _planes[index]; }
    };
}

#endif //WOW_UNIX_FRUSTUM_H
