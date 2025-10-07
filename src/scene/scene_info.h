#ifndef GLAD_SCENE_INFO_H
#define GLAD_SCENE_INFO_H

#include <glm/vec3.hpp>

namespace wow::scene {
    struct scene_info {
        glm::vec3 camera_position{};
        float view_distance{};
    };
}

#endif //GLAD_SCENE_INFO_H