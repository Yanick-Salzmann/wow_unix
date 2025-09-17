#include "program.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <fstream>

#include "glm/gtc/type_ptr.hpp"
#include "spdlog/spdlog.h"

namespace wow::gl {
    program::program() {
        _program = glCreateProgram();
        _vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    program::~program() {
        glDeleteProgram(_program);
        glDeleteShader(_vertex_shader);
        glDeleteShader(_fragment_shader);
    }

    void program::use() const {
        glUseProgram(_program);
    }

    void program::unuse() {
        glUseProgram(0);
    }

    const program &program::compile_vertex_shader_from_file(const char *file_path) const {
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) {
            SPDLOG_ERROR("Failed to open vertex shader file: {}", file_path);
            return *this;
        }

        const std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        compile_shader(_vertex_shader, source.c_str(), "Vertex");
        return *this;
    }

    const program &program::compile_fragment_shader_from_file(const char *file_path) const {
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) {
            SPDLOG_ERROR("Failed to open fragment shader file: {}", file_path);
            return *this;
        }

        const std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        compile_shader(_fragment_shader, source.c_str(), "Fragment");
        return *this;
    }

    void program::compile_shader(const GLuint shader, const char *source, const char *shader_type) {
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length + 1);
            glGetShaderInfoLog(shader, length, &length, log.data());
            SPDLOG_ERROR("{} shader compilation failed: {}", shader_type, log.data());
        }
    }

    const program &program::compile_vertex_shader(const char *source) const {
        compile_shader(_vertex_shader, source, "Vertex");
        return *this;
    }

    const program &program::compile_fragment_shader(const char *source) const {
        compile_shader(_fragment_shader, source, "Fragment");
        return *this;
    }

    void program::link() const {
        glAttachShader(_program, _vertex_shader);
        glAttachShader(_program, _fragment_shader);
        glLinkProgram(_program);

        GLint status;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            GLint length;
            glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length + 1);
            glGetProgramInfoLog(_program, length, &length, log.data());
        }
    }

    int program::uniform_location(const char *name) const {
        return glGetUniformLocation(_program, name);
    }

    program &program::mat3(const glm::mat3 &matrix, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        return *this;
    }

    program &program::mat4(const glm::mat4 &matrix, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        return *this;
    }

    program &program::vec3(const glm::vec3 &vector, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform3fv(location, 1, glm::value_ptr(vector));
        return *this;
    }

    program &program::vec4(const glm::vec4 &vector, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform4fv(location, 1, glm::value_ptr(vector));
        return *this;
    }

    program &program::float_(const float value, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform1f(location, value);
        return *this;
    }

    program &program::int_(const int value, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform1i(location, value);
        return *this;
    }

    program &program::bool_(const bool value, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform1i(location, value);
        return *this;
    }

    program &program::sampler2d(const int index, const char *name) {
        const auto location = glGetUniformLocation(_program, name);
        glUniform1i(location, index);
        return *this;
    }

    program & program::sampler2d(int index, int location) {
        glUniform1i(location, index);
        return *this;
    }
}
