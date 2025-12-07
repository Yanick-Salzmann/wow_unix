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

    const program &program::compile_vertex_shader_from_file(const std::string &file_path) const {
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) {
            SPDLOG_ERROR("Failed to open vertex shader file: {}", file_path);
            return *this;
        }

        const std::string source((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
        compile_shader(_vertex_shader, source.c_str(), "Vertex");
        return *this;
    }

    const program &program::compile_fragment_shader_from_file(const std::string &file_path) const {
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) {
            SPDLOG_ERROR("Failed to open fragment shader file: {}", file_path);
            return *this;
        }

        const std::string source((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
        compile_shader(_fragment_shader, source.c_str(), "Fragment");
        return *this;
    }

    void program::compile_shader(const GLuint shader, const std::string &source, const std::string &shader_type) {
        const auto src_ptr = source.c_str();
        glShaderSource(shader, 1, &src_ptr, nullptr);
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

    const program &program::compile_vertex_shader(const std::string &source) const {
        compile_shader(_vertex_shader, source, "Vertex");
        return *this;
    }

    const program &program::compile_fragment_shader(const std::string &source) const {
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
            SPDLOG_ERROR("Error linking program: {}", log.data());
            throw std::runtime_error("Error linking program");
        }
    }

    int32_t program::uniform_location(const std::string &name) const {
        return glGetUniformLocation(_program, name.c_str());
    }

    int32_t program::attribute_location(const std::string &name) const {
        return glGetAttribLocation(_program, name.c_str());
    }

    program &program::mat3(const glm::mat3 &matrix, const std::string &name) {
        return mat3(matrix, uniform_location(name));
    }

    program &program::mat3(const glm::mat3 &matrix, int32_t location) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        return *this;
    }

    program &program::mat4(const glm::mat4 &matrix, const std::string &name) {
        return mat4(matrix, uniform_location(name));
    }

    program &program::mat4(const glm::mat4 &matrix, int32_t location) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        return *this;
    }

    program &program::vec3(const glm::vec3 &vector, const std::string &name) {
        return vec3(vector, uniform_location(name));
    }

    program &program::vec3(const glm::vec3 &vector, int32_t location) {
        glUniform3fv(location, 1, glm::value_ptr(vector));
        return *this;
    }

    program &program::vec4(const glm::vec4 &vector, const std::string &name) {
        return vec4(vector, uniform_location(name));
    }

    program &program::vec4(const glm::vec4 &vector, int32_t location) {
        glUniform4fv(location, 1, glm::value_ptr(vector));
        return *this;
    }

    program &program::float_(const float value, const std::string &name) {
        return float_(value, uniform_location(name));
    }

    program &program::float_(float value, int32_t location) {
        glUniform1f(location, value);
        return *this;
    }

    program &program::int_(const int value, const std::string &name) {
        return int_(value, uniform_location(name));
    }

    program &program::int_(int value, int32_t location) {
        glUniform1i(location, value);
        return *this;
    }

    program &program::bool_(const bool value, const std::string &name) {
        return bool_(value, uniform_location(name));
    }

    program &program::bool_(bool value, int32_t location) {
        glUniform1i(location, value);
        return *this;
    }

    program &program::sampler2d(const int index, const std::string &name) {
        const auto location = glGetUniformLocation(_program, name.c_str());
        glUniform1i(location, index);
        return *this;
    }

    program &program::sampler2d(int index, int location) {
        glUniform1i(location, index);
        return *this;
    }
}
