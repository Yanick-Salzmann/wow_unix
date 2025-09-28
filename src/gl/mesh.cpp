#include "mesh.h"

#include <memory>
#include <utility>

#include "spdlog/spdlog.h"

namespace wow::gl {
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

    mesh &mesh::add_vertex_attribute(GLuint index, GLint size, GLenum type,
                                     GLboolean normalized, GLsizei stride, const void *offset) {
        _vertex_attributes.push_back({index, size, type, normalized, stride, offset});
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
        auto loc = program()->uniform_location(name.c_str());
        if (loc < 0) {
            SPDLOG_ERROR("Texture {} not found in program", name);
            throw std::runtime_error("Texture not found in program");
        }

        _textures[loc] = texture;
        return *this;
    }

    void mesh::bind() const {
        glBindVertexArray(_vao);
        if (_vertex_buffer) {
            _vertex_buffer->bind();
        }
        if (_index_buffer) {
            _index_buffer->bind();
        }
        if (_program) {
            _program->use();
        }

        int index = 0;
        for (const auto &[uniform, texture]: _textures) {
            glActiveTexture(GL_TEXTURE0 + index);
            texture->bind();
            _program->sampler2d(index, uniform);
            index++;
        }

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

    void mesh::draw() const {
        bind();

        if (_index_buffer && _index_count > 0) {
            glDrawElements(_primitive_type, static_cast<GLsizei>(_index_count), _index_buffer->type(), nullptr);
        } else {
            glDrawArrays(_primitive_type, 0, static_cast<GLsizei>(_index_count));
        }
    }

    void mesh::draw_instanced(GLsizei instance_count) const {
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

        for (const auto &[
                 index,
                 size,
                 type,
                 normalized,
                 stride,
                 offset
             ]: _vertex_attributes) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, type, normalized, stride, offset);
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

        const auto ib = std::make_shared<gl::index_buffer>(GL_UNSIGNED_INT);
        ib->set_data(indices, sizeof(indices));

        const auto program = std::make_shared<gl::program>();
        program->compile_vertex_shader_from_file("shaders/ui_vertex.glsl")
                .compile_fragment_shader_from_file("shaders/ui_fragment.glsl")
                .link();

        quad_mesh->vertex_buffer(vb)
                .index_buffer(ib)
                .set_index_count(6)
                .add_vertex_attribute(0, 2,GL_FLOAT, false, 4 * sizeof(float), nullptr)
                .add_vertex_attribute(1, 2,GL_FLOAT, false, 4 * sizeof(float), (void *) (2 * sizeof(float)))
                .program(program);

        return quad_mesh;
    }

    mesh_ptr mesh::terrain_mesh() {
        static auto mesh = std::make_shared<gl::mesh>();
        static std::once_flag flag{};
        std::call_once(flag, [] {
            const auto program = std::make_shared<gl::program>();
            program->compile_vertex_shader_from_file("shaders/terrain_vertex.glsl")
                    .compile_fragment_shader_from_file("shaders/terrain_fragment.glsl")
                    .link();

            mesh->set_index_count(768)
                    .add_vertex_attribute(0, 3, GL_FLOAT, false, 10 * sizeof(float), nullptr)
                    .add_vertex_attribute(0, 3, GL_FLOAT, false, 10 * sizeof(float), (void *) (3 * sizeof(float)))
                    .add_vertex_attribute(0, 2, GL_FLOAT, false, 10 * sizeof(float), (void *) (6 * sizeof(float)))
                    .add_vertex_attribute(0, 2, GL_FLOAT, false, 10 * sizeof(float), (void *) (8 * sizeof(float)))
                    .program(program);
        });

        return mesh;
    }
}
