#include "math.h"

namespace wow::utils {
    bool bounding_box::intersects_sphere(const glm::vec3 &center, const float radius) const {
        const auto r_squared = radius * radius;
        auto d_min = 0.0f;
        for (auto i = 0; i < 3; ++i) {
            if (center[i] < _min[i]) {
                d_min += (center[i] - _min[i]) * (center[i] - _min[i]);
            } else if (center[i] > _max[i]) {
                d_min += (center[i] - _max[i]) * (center[i] - _max[i]);
            }
        }

        return d_min <= r_squared;
    }
}
