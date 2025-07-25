#include "renderer.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include "leper/leper_rendering_constants.h"

namespace leper {

    Renderer::Renderer() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            spdlog::error("Failed to initalize GLAD");
        }

        glGenFramebuffers(1, &main_fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, main_fbo_);

        // Color texture
        glGenTextures(1, &color_tex_);
        glBindTexture(GL_TEXTURE_2D, color_tex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Depth texture
        glGenRenderbuffers(1, &depth_rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT);

        // Attach
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex_, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Framebuffer is not complete!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::start_frame() {
        glBindFramebuffer(GL_FRAMEBUFFER, main_fbo_);
        glViewport(0, 0, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move this in dedicated methods
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void Renderer::finish_frame(uint16_t width, uint16_t height) {
        // Set default framebuffer to screen
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, main_fbo_);

        glBlitFramebuffer(0, 0, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT,
                          0, 0, width, height,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    bool Renderer::has_mesh_objects(const Mesh& mesh) {
        return mesh_objects_.find(mesh.name) != mesh_objects_.end();
    }

    void Renderer::upload_mesh(const Mesh& mesh) {
        assert(!has_mesh_objects(mesh) && "Creating the same mesh objects twice");

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

        mesh_objects_.insert({mesh.name, MeshGlObjetcs{
                                             .vao = vao,
                                             .ebo = ebo}});
    }

    void Renderer::draw_mesh(const Mesh& mesh) {
        if (mesh_objects_.find(mesh.name) != mesh_objects_.end()) {
            const auto objects = mesh_objects_.at(mesh.name);

            glBindVertexArray(objects.vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
            glBindVertexArray(0);
        } else {
            spdlog::warn("Tried to draw an unuploaded mesh");
        }
    }

    Renderer::~Renderer() {

        glDeleteRenderbuffers(1, &depth_rbo_);
        glDeleteTextures(1, &color_tex_);
        glDeleteFramebuffers(1, &main_fbo_);
        // TODO: destroy all objects
    }

} // namespace leper
