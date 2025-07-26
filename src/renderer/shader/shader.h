#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace leper {

    enum class CompilationStepCheck {
        Fragment,
        Vertex,
        Program,
    };

    inline const char* to_string(CompilationStepCheck step) {
        switch (step) {
        case CompilationStepCheck::Fragment:
            return "Fragment";
        case CompilationStepCheck::Vertex:
            return "Vertex";
        case CompilationStepCheck::Program:
            return "Program";
        default:
            return "Unknown";
        }
    }

    class Shader {
      public:
        Shader(const std::string& vertex_shader_name, const std::string& fragment_shader_name);
        void bind();
        /*
            void unbind();

            void set_uniform_1i(const std::string& name, int i);
        */
        void set_uniform_1f(const std::string& name, float f);
        void set_uniform_vec3f(const std::string& name, glm::vec3 v);
        void set_uniform_mat4f(const std::string& name, glm::mat4 m);

      private:
        static void check_compilation_errors(GLuint shader, CompilationStepCheck step);
        static std::string read_shader_file(const std::string& file_name);
        GLuint compile(const char* vertCode, const char* fragCode);

        GLuint program_ = 0;
    };

} // namespace leper
