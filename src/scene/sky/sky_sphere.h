#ifndef GLAD_SKY_SPHERE_H
#define GLAD_SKY_SPHERE_H

#include "gl/mesh.h"
#include "glm/vec3.hpp"

namespace wow::scene::sky {
    class sky_sphere {
        gl::mesh_ptr _mesh{};
        gl::vertex_buffer_ptr _vertex_buffer{};
        gl::index_buffer_ptr _index_buffer{};

    public:
        void initialize();

        void on_frame();
        void update_position(const glm::vec3& position);
        void update_radius(float radius);
    };
}

#endif //GLAD_SKY_SPHERE_H