#include "renderer.h"

#include <glad/glad.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include "leper/leper_rendering_constants.h"

namespace leper {

    Renderer::Renderer() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            spdlog::error("Failed to initalize GLAD");
        }
        init_main_frame();
        init_shadow_map();
        depth_shader_ = std::make_unique<Shader>("depth.vert.glsl", "depth.frag.glsl");
    }

    void Renderer::init_main_frame() {
        glGenFramebuffers(1, &main_fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, main_fbo_);

        // Color
        glGenTextures(1, &main_texture_);
        glBindTexture(GL_TEXTURE_2D, main_texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Depth
        glGenRenderbuffers(1, &main_depth_rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, main_depth_rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT);

        // Attach
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_texture_, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_depth_rbo_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Framebuffer is not complete!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::init_shadow_map() {
        glGenFramebuffers(1, &shadow_map_fbo_);

        glGenTextures(1, &shadow_map_);
        glBindTexture(GL_TEXTURE_2D, shadow_map_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Shadow map framebuffer is not complete!");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::start_shadow_frame() {
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo_);
        glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

        glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::start_main_frame() {
        glBindFramebuffer(GL_FRAMEBUFFER, main_fbo_);
        glViewport(0, 0, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move this in dedicated methods
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB);

        glBindTexture(GL_TEXTURE_2D, shadow_map_);
    }

    void Renderer::finish_main_frame(uint16_t width, uint16_t height) {
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

    Shader* Renderer::get_depth_shader() {
        return depth_shader_.get();
    }

    void Renderer::reload_shaders() {
        for (auto& shader_pair : shaders_) {
            shader_pair.second.reload();
        }
    }

    Renderer::~Renderer() {

        glDeleteRenderbuffers(1, &main_depth_rbo_);
        glDeleteTextures(1, &main_texture_);
        glDeleteFramebuffers(1, &main_fbo_);

        glDeleteTextures(1, &shadow_map_);
        glDeleteFramebuffers(1, &shadow_map_fbo_);

        for (auto& shader_pair : shaders_) {
            shader_pair.second.cleanup();
        }

        for (auto& mesh_pair : mesh_objects_) {
            glDeleteVertexArrays(1, &mesh_pair.second.vao);
            glDeleteBuffers(1, &mesh_pair.second.ebo);
        }
    }

} // namespace leper
