#include "mesh.h"

#include <memory>
#include <utility>
#include <vector>
#include <cmath>

#include "spdlog/spdlog.h"

namespace wow::gl {
    void terrain_mesh::apply_fog_color(const glm::vec4 &fog_color) {
        mesh->program()->vec4(fog_color, state.fog_color);
    }

    mesh::mesh() {
        glGenVertexArrays(1, &_vao);
    }

    mesh::~mesh() {
        cleanup();
    }

    mesh::mesh(mesh &&other) noexcept
        : _vao(other._vao)
          , _index_buffer(std::move(other._index_buffer))
          , _vertex_buffer(std::move(other._vertex_buffer))
          , _program(std::move(other._program))
          , _vertex_attributes(std::move(other._vertex_attributes))
          , _index_count(other._index_count)
          , _primitive_type(other._primitive_type) {
        other._vao = 0;
        other._index_count = 0;
    }

    mesh &mesh::operator=(mesh &&other) noexcept {
        if (this != &other) {
            cleanup();

            _vao = other._vao;
            _index_buffer = std::move(other._index_buffer);
            _vertex_buffer = std::move(other._vertex_buffer);
            _program = std::move(other._program);
            _vertex_attributes = std::move(other._vertex_attributes);
            _index_count = other._index_count;
            _primitive_type = other._primitive_type;

            other._vao = 0;
            other._index_count = 0;
        }
        return *this;
    }

    mesh &mesh::vertex_buffer(vertex_buffer_ptr buffer) {
        _vertex_buffer = std::move(buffer);
        setup_vertex_attributes();
        return *this;
    }

    mesh &mesh::index_buffer(index_buffer_ptr buffer) {
        _index_buffer = std::move(buffer);
        return *this;
    }

    mesh &mesh::program(program_ptr program) {
        _program = std::move(program);
        return *this;
    }

    mesh &mesh::add_vertex_attribute(std::string name, const GLuint index, const GLint size, const GLenum type,
                                     const GLboolean normalized, const GLsizei stride, const void *offset) {
        _vertex_attributes.push_back({-1, std::move(name), index, size, type, normalized, stride, offset});
        if (_vertex_buffer) {
            setup_vertex_attributes();
        }
        return *this;
    }

    mesh &mesh::set_index_count(size_t count) {
        _index_count = count;
        return *this;
    }

    mesh &mesh::set_primitive_type(GLenum type) {
        _primitive_type = type;
        return *this;
    }

    mesh &mesh::texture(const std::string &name, const bindable_texture_ptr &texture) {
        const auto loc = program()->uniform_location(name);
        if (loc < 0) {
            SPDLOG_ERROR("Texture {} not found in program", name);
            throw std::runtime_error("Texture not found in program");
        }

        return this->texture(loc, texture);
    }

    mesh &mesh::texture(const int32_t location, const bindable_texture_ptr &texture) {
        _textures[location] = texture;
        return *this;
    }

    const mesh &mesh::bind() const {
        bind_vertex_attributes();
        bind_vb();
        bind_ib();
        bind_program();
        bind_textures();
        apply_blend_mode();

        return *this;
    }

    const mesh &mesh::bind_textures() const {
        int index = 0;
        for (const auto &[uniform, texture]: _textures) {
            glActiveTexture(GL_TEXTURE0 + index);
            texture->bind();
            _program->sampler2d(index, uniform);
            index++;
        }

        return *this;
    }

    const mesh &mesh::bind_vb() const {
        if (_vertex_buffer) {
            _vertex_buffer->bind();
        }

        return *this;
    }

    const mesh &mesh::bind_ib() const {
        if (_index_buffer) {
            _index_buffer->bind();
        }

        return *this;
    }

    const mesh &mesh::bind_program() const {
        if (_program) {
            _program->use();
        }

        return *this;
    }

    const mesh &mesh::bind_vertex_attributes() const {
        glBindVertexArray(_vao);
        return *this;
    }

    const mesh &mesh::apply_blend_mode() const {
        switch (_blend_mode) {
            case blend_mode::none:
                glDisable(GL_BLEND);
                break;

            case blend_mode::alpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;

            case blend_mode::color:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                break;
        }

        return *this;
    }

    void mesh::unbind() const {
        glBindVertexArray(0);
        if (_vertex_buffer) {
            _vertex_buffer->unbind();
        }
        if (_index_buffer) {
            _index_buffer->unbind();
        }
        program::unuse();
    }

    void mesh::draw(bool skip_bind, uint32_t offset) const {
        if (!skip_bind) {
            bind();
        }

        if (_index_buffer && _index_count > 0) {
            if (offset <= 0) {
                glDrawElements(_primitive_type, static_cast<GLsizei>(_index_count), _index_buffer->type(), nullptr);
            } else {
                glDrawElementsBaseVertex(_primitive_type, static_cast<GLsizei>(_index_count), _index_buffer->type(),
                                         nullptr, offset);
            }
        } else {
            glDrawArrays(_primitive_type, offset, static_cast<GLsizei>(_index_count));
        }
    }

    void mesh::draw_instanced(const GLsizei instance_count) const {
        bind();

        if (_index_buffer && _index_count > 0) {
            glDrawElementsInstanced(_primitive_type, static_cast<GLsizei>(_index_count),
                                    _index_buffer->type(), nullptr, instance_count);
        } else {
            glDrawArraysInstanced(_primitive_type, 0, static_cast<GLsizei>(_index_count), instance_count);
        }
    }

    void mesh::setup_vertex_attributes() {
        if (!_vertex_buffer || _vertex_attributes.empty()) {
            return;
        }

        glBindVertexArray(_vao);
        _vertex_buffer->bind();

        for (auto &[
                 location,
                 name,
                 index,
                 size,
                 type,
                 normalized,
                 stride,
                 offset
             ]: _vertex_attributes) {
            if (location < 0) {
                _program->use();
                location = _program->attribute_location(fmt::format("{}{}", name, index));
            }
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, size, type, normalized, stride, offset);
        }

        glBindVertexArray(0);
    }

    void mesh::cleanup() {
        if (_vao != 0) {
            glDeleteVertexArrays(1, &_vao);
            _vao = 0;
        }
    }

    mesh_ptr mesh::create_ui_quad() {
        auto quad_mesh = make_mesh();

        constexpr float vertices[] = {
            -1.0f, -1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f
        };

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        const auto vb = std::make_shared<gl::vertex_buffer>();
        vb->set_data(vertices, sizeof(vertices));

        const auto ib = std::make_shared<gl::index_buffer>(index_type::uint32);
        ib->set_data(indices, sizeof(indices));

        const auto program = std::make_shared<gl::program>();
        program->compile_vertex_shader_from_file("shaders/ui_vertex.glsl")
                .compile_fragment_shader_from_file("shaders/ui_fragment.glsl")
                .link();

        quad_mesh->
                add_vertex_attribute("vertex_position", 0, 2,GL_FLOAT, false, 4 * sizeof(float), nullptr)
                .add_vertex_attribute("vertex_texcoord", 0, 2,GL_FLOAT, false, 4 * sizeof(float),
                                      reinterpret_cast<void *>(2 * sizeof(float)))

                .program(program)
                .vertex_buffer(vb)
                .index_buffer(ib)
                .set_index_count(6);

        return quad_mesh;
    }

    terrain_mesh mesh::terrain_mesh() {
        static struct terrain_mesh ret_mesh{};
        static std::once_flag flag{};
        std::call_once(flag, [] {
            const auto program = std::make_shared<gl::program>();
            program->compile_vertex_shader_from_file("shaders/terrain_vertex.glsl")
                    .compile_fragment_shader_from_file("shaders/terrain_fragment.glsl")
                    .link();

            auto mesh = make_mesh();

            mesh->set_index_count(768)
                    .add_vertex_attribute("position", 0, 3, GL_FLOAT, false, 13 * sizeof(float))
                    .add_vertex_attribute("normal", 0, 3, GL_FLOAT, false, 13 * sizeof(float),
                                          reinterpret_cast<void *>(3 * sizeof(float)))
                    .add_vertex_attribute("tex_coord", 0, 2, GL_FLOAT, false, 13 * sizeof(float),
                                          reinterpret_cast<void *>(6 * sizeof(float)))
                    .add_vertex_attribute("alpha_coord", 0, 2, GL_FLOAT, false, 13 * sizeof(float),
                                          reinterpret_cast<void *>(8 * sizeof(float)))
                    .add_vertex_attribute("vertex_color", 0, 3, GL_FLOAT, false, 13 * sizeof(float),
                                          reinterpret_cast<void *>(10 * sizeof(float)))
                    .program(program);

            ret_mesh.mesh = std::move(mesh);
            ret_mesh.state.camera_position = ret_mesh.mesh->program()->uniform_location("camera_position");
            ret_mesh.state.fog_color = ret_mesh.mesh->program()->uniform_location("fog_color");
        });

        return ret_mesh;
    }

    mesh_ptr mesh::sky_sphere_mesh() {
        static auto mesh = std::make_shared<gl::mesh>();
        static std::once_flag flag{};
        std::call_once(flag, [] {
            constexpr int stacks = 16;
            constexpr int slices = 32;
            std::vector<float> vertices;
            vertices.reserve((stacks + 1) * (slices + 1) * 3);

            for (int i = 0; i <= stacks; ++i) {
                const float v = static_cast<float>(i) / static_cast<float>(stacks);
                const float phi = v * static_cast<float>(M_PI);
                const float z = std::cos(phi);
                const float r = std::sin(phi);
                for (int j = 0; j <= slices; ++j) {
                    const float u = static_cast<float>(j) / static_cast<float>(slices);
                    const float theta = u * static_cast<float>(M_PI) * 2.0f;
                    const float x = r * std::cos(theta);
                    const float y = r * std::sin(theta);
                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(z);
                }
            }

            std::vector<uint32_t> indices;
            indices.reserve(stacks * slices * 6);
            constexpr int stride = slices + 1;
            for (int i = 0; i < stacks; ++i) {
                for (int j = 0; j < slices; ++j) {
                    const uint32_t i0 = i * stride + j;
                    const uint32_t i1 = i0 + 1;
                    const uint32_t i2 = (i + 1) * stride + j;
                    const uint32_t i3 = i2 + 1;

                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i1);

                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }

            const auto vb = std::make_shared<gl::vertex_buffer>();
            vb->set_data(vertices.data(), vertices.size() * sizeof(float));

            const auto ib = std::make_shared<gl::index_buffer>(index_type::uint32);
            ib->set_data(indices.data(), indices.size() * sizeof(uint32_t));

            const auto program = std::make_shared<gl::program>();
            program->compile_vertex_shader_from_file("shaders/sky_vertex.glsl")
                    .compile_fragment_shader_from_file("shaders/sky_fragment.glsl")
                    .link();

            mesh->vertex_buffer(vb)
                    .index_buffer(ib)
                    .set_index_count(indices.size())
                    .program(program);

            mesh->add_vertex_attribute("position", 0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
        });

        return mesh;
    }
}
