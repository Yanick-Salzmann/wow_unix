#ifndef WOW_UNIX_SKY_SPHERE_H
#define WOW_UNIX_SKY_SPHERE_H

#include "gl/mesh.h"
#include "glm/vec3.hpp"
#include "utils/common_utils.hpp"

namespace wow::scene::sky {
    class sky_sphere {
        std::array<uint32_t, 180> _sky_gradient{};

        gl::mesh_ptr _mesh{};
        gl::vertex_buffer_ptr _vertex_buffer{};
        gl::index_buffer_ptr _index_buffer{};
        gl::texture_ptr _sky_texture{};

        glm::vec3 _position{};
        float _radius = 1.0f;

        void calculate_buffer() const;

    public:
        void initialize();

        void on_frame() const;
        void update_position(const glm::vec3& position);
        void update_radius(float radius);

        void update_matrix(const glm::mat4& view, const glm::mat4& projection) const;

        void update_gradient(
            const glm::vec4& top_color,
            const glm::vec4& middle_color,
            const glm::vec4& band1_color,
            const glm::vec4& band2_color,
            const glm::vec4& smog_color,
            const glm::vec4& fog_color
            );
    };

    typedef std::shared_ptr<sky_sphere> sky_sphere_ptr;

    SHARED_PTR_FUNC_NAME(sky_sphere);
}

#endif //WOW_UNIX_SKY_SPHERE_H