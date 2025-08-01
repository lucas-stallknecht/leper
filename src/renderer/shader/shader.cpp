#include "shader.h"

#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace leper {

    Shader::Shader(const std::string& vertex_shader_name, const std::string& fragment_shader_name)
        : vertex_shader_name_(vertex_shader_name), fragment_shader_name_(fragment_shader_name) {

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
                spdlog::error("Shader Compilation Error [{}]:\n{}\n",
                              to_string(step), info_log);
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                spdlog::error("Program Link Error [{}]:\n{}\n",
                              to_string(step), info_log);
            }
        }
    }

    std::string Shader::read_shader_file(const std::string& file_name) {
        std::filesystem::path shader_file_path = std::filesystem::path(SHADER_DIR) / file_name;
        std::ifstream file(shader_file_path);

        if (!file.is_open()) {
            spdlog::error("Failed to open shader file {}", shader_file_path.string());
        }

        std::stringstream shaderStream;
        shaderStream << file.rdbuf();
        file.close();
        return shaderStream.str();
    }

    GLuint Shader::compile(const char* vertex_code, const char* fragment_code) {

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_code, nullptr);
        glCompileShader(vertex_shader);
        Shader::check_compilation_errors(vertex_shader, CompilationStepCheck::Vertex);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_code, nullptr);
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

    void Shader::set_uniform_1i(const std::string& name, int i) {
        int loc = glGetUniformLocation(program_, name.c_str());
        if (loc == -1) {
            spdlog::error("Uniform location not found: {}", name);
        }
        glUniform1i(loc, i);
    }

    void Shader::set_uniform_1f(const std::string& name, float f) {
        int loc = glGetUniformLocation(program_, name.c_str());
        if (loc == -1) {
            spdlog::error("Uniform location not found: {}", name);
        }
        glUniform1f(loc, f);
    }

    void Shader::set_uniform_vec3f(const std::string& name, glm::vec3 v) {
        int loc = glGetUniformLocation(program_, name.c_str());
        if (loc == -1) {
            spdlog::error("Uniform location not found: {}", name);
        }
        glUniform3fv(loc, 1, glm::value_ptr(v));
    }

    void Shader::set_uniform_mat4f(const std::string& name, glm::mat4 m) {
        int loc = glGetUniformLocation(program_, name.c_str());
        if (loc == -1) {
            spdlog::error("Uniform location not found: {}", name);
        }
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    }

    void Shader::reload() {
        const std::string vertex_code = Shader::read_shader_file(vertex_shader_name_);
        const std::string fragment_code = Shader::read_shader_file(fragment_shader_name_);

        program_ = compile(vertex_code.c_str(), fragment_code.c_str());

        GLuint new_program = compile(vertex_code.c_str(), fragment_code.c_str());
        if (new_program != 0) {
            glDeleteProgram(program_);
            program_ = new_program;
        }
    }

    void Shader::cleanup() {
        glDeleteProgram(program_);
    }

} // namespace leper
