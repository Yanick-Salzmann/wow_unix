#ifndef WOW_UNIX_MESH_H
#define WOW_UNIX_MESH_H

#include <map>

#include "index_buffer.h"
#include "program.h"
#include "vertex_buffer.h"
#include "texture.h"
#include <glad/gl.h>
#include <vector>

namespace wow::gl {
    struct vertex_attribute {
        int32_t location = -1;
        std::string name;
        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void *offset;
    };

    class mesh;

    using mesh_ptr = std::shared_ptr<mesh>;

    enum class blend_mode {
        none,
        alpha,
        color
    };

    class mesh {
        GLuint _vao{};

        index_buffer_ptr _index_buffer{};
        vertex_buffer_ptr _vertex_buffer{};

        program_ptr _program{};
        std::vector<vertex_attribute> _vertex_attributes{};

        size_t _index_count{};
        GLenum _primitive_type{GL_TRIANGLES};

        blend_mode _blend_mode{blend_mode::none};

        std::map<int, bindable_texture_ptr> _textures{};

    public:
        mesh();

        ~mesh();

        mesh(const mesh &) = delete;

        mesh &operator=(const mesh &) = delete;

        mesh(mesh &&other) noexcept;

        mesh &operator=(mesh &&other) noexcept;

        mesh &vertex_buffer(vertex_buffer_ptr buffer);

        mesh &index_buffer(index_buffer_ptr buffer);

        mesh &program(program_ptr program);

        mesh &blend(blend_mode mode) {
            _blend_mode = mode;
            return *this;
        }

        mesh &add_vertex_attribute(std::string name, GLuint index, GLint size, GLenum type,
                                   GLboolean normalized = GL_FALSE,
                                   GLsizei stride = 0,
                                   const void *offset = nullptr);

        mesh &set_index_count(size_t count);

        mesh &set_primitive_type(GLenum type);

        mesh &texture(const std::string &name, const bindable_texture_ptr &texture);

        mesh &texture(int32_t location, const bindable_texture_ptr &texture);

        void bind() const;

        void unbind() const;

        void draw() const;

        void draw_instanced(GLsizei instance_count) const;

        // Getters
        [[nodiscard]] const vertex_buffer_ptr &vertex_buffer() const { return _vertex_buffer; }
        [[nodiscard]] const index_buffer_ptr &index_buffer() const { return _index_buffer; }
        [[nodiscard]] const program_ptr &program() const { return _program; }
        [[nodiscard]] size_t index_count() const { return _index_count; }
        [[nodiscard]] GLenum primitive_type() const { return _primitive_type; }

        static mesh_ptr create_ui_quad();

        static mesh_ptr terrain_mesh();

    private:
        void setup_vertex_attributes();

        void cleanup();
    };

    inline mesh_ptr make_mesh() {
        return std::make_shared<mesh>();
    }
}

#endif //WOW_UNIX_MESH_H
