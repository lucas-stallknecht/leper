#include "renderer.h"
#include "leper/leper_rendering_types.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

namespace leper {

    Renderer::Renderer() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            spdlog::error("Failed to initalize GLAD");
        }
    }

    void Renderer::register_mesh(const Mesh& mesh) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        GLuint vao;
        glGenVertexArrays(1, &vao);
        GLuint ebo;
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(leper::Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(leper::Vertex), (void*)offsetof(leper::Vertex, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(leper::Vertex), (void*)offsetof(leper::Vertex, normal));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vbo);

        mesh_objects_.insert({&mesh, MeshGlObjetcs{
                                         .vao = vao,
                                         .ebo = ebo}});
    }

    void Renderer::draw_mesh(const Mesh& mesh) {
        if (mesh_objects_.find(&mesh) != mesh_objects_.end()) {
            const auto objects = mesh_objects_.at(&mesh);

            glBindVertexArray(objects.vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        } else {
            spdlog::warn("Tried to draw an unregistered mesh");
        }
    }

} // namespace leper
