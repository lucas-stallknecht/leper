#include "shader.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace leper {

    Shader::Shader(const std::string& vertex_shader_name, const std::string& fragment_shader_name) {
        const std::string vertex_code = Shader::read_shader_file(vertex_shader_name);
        const std::string fragment_code = Shader::read_shader_file(fragment_shader_name);

        program_ = compile(vertex_code.c_str(), fragment_code.c_str());
    }

    void Shader::check_compilation_errors(GLuint shader, CompilationStepCheck step) {
        int success;
        char info_log[1024];

        if (step != CompilationStepCheck::Program) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, info_log);
                spdlog::error("SHADER COMPILATION ERROR [{}]:\n{}\n-- --------------------------------------------------- --",
                              to_string(step), info_log);
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                spdlog::error("PROGRAM LINKING ERROR [{}]:\n{}\n-- --------------------------------------------------- --",
                              to_string(step), info_log);
            }
        }
    }

    std::string Shader::read_shader_file(const std::string& file_name) {
        std::filesystem::path shader_file_path = std::filesystem::path(SHADER_DIR) / file_name;
        std::ifstream file(shader_file_path);

        if (!file.is_open()) {
            spdlog::error("Faield to open shader file {}", shader_file_path.string());
        }

        std::stringstream shaderStream;
        shaderStream << file.rdbuf();
        file.close();
        return shaderStream.str();
    }

    GLuint Shader::compile(const char* vertex_code, const char* fragment_code) {

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_code, NULL);
        glCompileShader(vertex_shader);
        Shader::check_compilation_errors(vertex_shader, CompilationStepCheck::Vertex);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_code, NULL);
        glCompileShader(fragment_shader);
        Shader::check_compilation_errors(fragment_shader, CompilationStepCheck::Fragment);

        GLuint shader_program;
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        Shader::check_compilation_errors(fragment_shader, CompilationStepCheck::Program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return shader_program;
    }

    void Shader::bind() {
        glUseProgram(program_);
    }

    void Shader::set_uniform_4m(const std::string& name, glm::mat4 m) {
        // TODO: assert
        uint32_t loc = glGetUniformLocation(program_, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    }

} // namespace leper
