#include "sky_sphere.h"
#include <vector>
#include <glm/gtc/constants.hpp>

namespace wow::scene::sky {
    constexpr int32_t horizontal_resolution = 100;
    constexpr int32_t vertical_resolution = 100;

#pragma pack(push, 1)
    struct sky_vertex {
        glm::vec3 position{};
        glm::vec2 tex_coord{};
    };
#pragma pack(pop)

    void sky_sphere::calculate_buffer() {
        std::vector<sky_vertex> vertices;
        vertices.reserve((horizontal_resolution + 1) * (vertical_resolution + 1));

        for (auto i = 0; i <= vertical_resolution; ++i) {
            const auto v = static_cast<float>(i) / static_cast<float>(vertical_resolution);
            const auto phi = v * glm::pi<float>();

            for (auto j = 0; j <= horizontal_resolution; ++j) {
                const auto u = static_cast<float>(j) / static_cast<float>(horizontal_resolution);
                const auto theta = u * glm::two_pi<float>();

                const auto x = std::sin(phi) * std::cos(theta);
                const auto y = std::cos(phi);
                const auto z = std::sin(phi) * std::sin(theta);

                vertices.push_back({
                    _position + glm::vec3(x, y, z) * _radius,
                    glm::vec2(u, v)
                });
            }
        }

        _vertex_buffer->set_data(vertices);
    }

    void sky_sphere::initialize() {
        _mesh = gl::make_mesh();
        _vertex_buffer = gl::make_vertex_buffer();
        _index_buffer = gl::make_index_buffer(gl::index_type::uint16);

        calculate_buffer();

        std::vector<uint16_t> indices;
        indices.reserve(horizontal_resolution * vertical_resolution * 6);

        for (auto i = 0; i < vertical_resolution; ++i) {
            for (auto j = 0; j < horizontal_resolution; ++j) {
                const auto p1 = i * (horizontal_resolution + 1) + j;
                const auto p2 = p1 + horizontal_resolution + 1;
                const auto p3 = p1 + 1;
                const auto p4 = p2 + 1;

                indices.push_back(p1);
                indices.push_back(p2);
                indices.push_back(p3);

                indices.push_back(p3);
                indices.push_back(p2);
                indices.push_back(p4);
            }
        }

        const auto prog = gl::make_program();
        prog->compile_vertex_shader_from_file("shaders/sky_vertex.glsl")
                .compile_fragment_shader_from_file("shaders/sky_fragment.glsl")
                .link();

        _mesh->program(prog);

        _index_buffer->set_data(indices);

        _mesh->add_vertex_attribute("position", 0, 3, GL_FLOAT, false, sizeof(sky_vertex), nullptr)
                .add_vertex_attribute("tex_coord", 0, 2, GL_FLOAT, false, sizeof(sky_vertex),
                                      reinterpret_cast<void *>(offsetof(sky_vertex, tex_coord)))
                .vertex_buffer(_vertex_buffer)
                .index_buffer(_index_buffer)
                .blend(gl::blend_mode::none);
    }

    void sky_sphere::on_frame() {
        //_mesh->draw();
    }

    void sky_sphere::update_position(const glm::vec3 &position) {
        _position = position;
        calculate_buffer();
    }

    void sky_sphere::update_radius(float radius) {
        _radius = radius;
        calculate_buffer();
    }
}
