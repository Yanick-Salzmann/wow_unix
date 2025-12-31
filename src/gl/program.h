#ifndef WOW_UNIX_PROGRAM_H
#define WOW_UNIX_PROGRAM_H

#include <memory>
#include <string>

extern "C" {
#include <glad/gl.h>
}

#include <glm/glm.hpp>

namespace wow::gl {
    class program {
        GLuint _program{};
        GLuint _vertex_shader{};
        GLuint _fragment_shader{};

        static void compile_shader(GLuint shader, const std::string &source, const std::string &shader_type);

    public:
        program();

        ~program();

        void use() const;

        static void unuse();

        const program &compile_vertex_shader_from_file(const std::string &file_path) const;

        const program &compile_fragment_shader_from_file(const std::string &file_path) const;

        const program &compile_vertex_shader(const std::string &source) const;

        const program &compile_fragment_shader(const std::string &source) const;

        void link() const;

        int32_t uniform_location(const std::string &name) const;

        int32_t attribute_location(const std::string &name) const;

        program &mat3(const glm::mat3 &matrix, const std::string &name);

        program &mat3(const glm::mat3 &matrix, int32_t location);

        program &mat4(const glm::mat4 &matrix, const std::string &name);

        program &mat4(const glm::mat4 &matrix, int32_t location);

        program &vec3(const glm::vec3 &vector, const std::string &name);

        program &vec3(const glm::vec3 &vector, int32_t location);

        program &vec4(const glm::vec4 &vector, const std::string &name);

        program &vec4(const glm::vec4 &vector, int32_t location);

        program &float_(float value, const std::string &name);

        program &float_(float value, int32_t location);

        program &int_(int value, const std::string &name);

        program &int_(int value, int32_t location);

        program &bool_(bool value, const std::string &name);

        program &bool_(bool value, int32_t location);

        program &sampler2d(int index, const std::string &name);

        program &sampler2d(int index, int location);
    };

    using program_ptr = std::shared_ptr<program>;

    inline program_ptr make_program() {
        return std::make_shared<program>();
    }
}

#endif //WOW_UNIX_PROGRAM_H
