#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sys/stat.h>

#include "asset_loading/obj_loading.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "renderer/shader/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {

    const uint32_t width = 1280u;
    const uint32_t height = 720u;

    const uint32_t downscaled_width = 320u;
    const uint32_t downscaled_height = 180u;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Leper", nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initalize GLAD");
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLsizei vertex_count = 0;
    auto cube_vertices = leper::load_obj_geometry("bunny.obj");
    if (!cube_vertices.has_value()) {
        spdlog::error("Failed to load OBJ model");
        return -1;
    }
    glBufferData(GL_ARRAY_BUFFER, cube_vertices.value().size() * sizeof(leper::Vertex), cube_vertices.value().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(leper::Vertex), (void*)offsetof(leper::Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(leper::Vertex), (void*)offsetof(leper::Vertex, normal));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    vertex_count = static_cast<GLsizei>(cube_vertices.value().size());

    leper::Shader simple_shader("simple.vert.glsl", "simple.frag.glsl");

    // Transformations
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
    float_t aspect = static_cast<float_t>(width) / static_cast<float>(height);
    float_t ortho_height = 1.0f;
    float_t ortho_width = ortho_height * aspect;

    glm::mat4 projection = glm::ortho(-ortho_width, ortho_width,
                                      -ortho_height, ortho_height,
                                      0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    // Light
    glm::vec3 sun_dir = glm::vec3{0.5f, 1.0f, 0.3f};

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Color texture that will hold main frame image
    GLuint color_tex;
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, downscaled_width, downscaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        glBindBuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, downscaled_width, downscaled_height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simple_shader.bind();
        simple_shader.set_uniform_vec3f("sunDir", sun_dir);
        model = glm::rotate(model, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 transform = projection * view * model;
        simple_shader.set_uniform_mat4f("transform", transform);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, downscaled_width, downscaled_height,
                          0, 0, width, height,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        glBindVertexArray(0);
        glBindBuffer(GL_FRAMEBUFFER, 0);
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &color_tex);
    glDeleteFramebuffers(1, &fbo);
    // TODO: destroy shader

    glfwTerminate();
    return 0;
}
