#include "sky_sphere.h"

namespace wow::scene::sky {
    void sky_sphere::initialize() {
        _mesh = gl::make_mesh();
        _vertex_buffer = gl::make_vertex_buffer();
        _index_buffer = gl::make_index_buffer(gl::index_type::uint16);

        /*_mesh->vertex_buffer(_vertex_buffer)
                .index_buffer(_index_buffer)
                .blend(gl::blend_mode::none);*/
    }

    void sky_sphere::on_frame() {
        //_mesh->draw();
    }

    void sky_sphere::update_position(const glm::vec3 &position) {
    }
}
