#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "asset_loading/obj_loading.h"
#include "glm/fwd.hpp"
#include "renderer/shader/shader.h"

int main() {

    const uint32_t width = 900u;
    const uint32_t height = 900u;

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

    glViewport(0, 0, width, height);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLsizei vertex_count = 0;
    auto cube_vertices = leper::load_obj_geometry("cube.obj");
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
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(0.4f, 0.4f, 0.4f));
    trans = glm::rotate(trans, glm::radians(45.f), glm::vec3(1.0, 1.0, 1.0));

    // Light
    glm::vec3 sun_dir = glm::vec3{0.5f, 1.0f, 0.3f};

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simple_shader.bind();
        simple_shader.set_uniform_vec3f("sunDir", sun_dir);
        simple_shader.set_uniform_mat4f("transform", trans);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    glfwTerminate();
    return 0;
}
