#ifndef WOW_UNIX_PROGRAM_H
#define WOW_UNIX_PROGRAM_H

#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace wow::gl {
    class program {
        GLuint _program{};
        GLuint _vertex_shader{};
        GLuint _fragment_shader{};

        static void compile_shader(GLuint shader, const char *source, const char *shader_type);

    public:
        program();

        ~program();

        void use() const;

        static void unuse();

        const program& compile_vertex_shader_from_file(const char *file_path) const;
        const program& compile_fragment_shader_from_file(const char *file_path) const;
        const program& compile_vertex_shader(const char *source) const;
        const program& compile_fragment_shader(const char *source) const;
        void link() const;

        int32_t uniform_location(const char *name) const;
        int32_t attribute_location(const char *name) const;

        program &mat3(const glm::mat3 &matrix, const char *name);

        program &mat4(const glm::mat4 &matrix, const char *name);

        program &vec3(const glm::vec3 &vector, const char *name);

        program &vec4(const glm::vec4 &vector, const char *name);

        program &float_(float value, const char *name);

        program &int_(int value, const char *name);

        program &bool_(bool value, const char *name);

        program &sampler2d(int index, const char *name);
        program& sampler2d(int index, int location);
    };

    using program_ptr = std::shared_ptr<program>;
}

#endif //WOW_UNIX_PROGRAM_H
