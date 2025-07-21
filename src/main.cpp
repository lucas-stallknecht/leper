#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "asset_loading/obj_loading.h"
#include "ecs/ecs.h"
#include "ecs/systems/transform_system.h"
#include "leper/leper_common_types.h"
#include "leper/leper_ecs_components.h"
#include "leper/leper_ecs_types.h"
#include "renderer/renderer.h"
#include "renderer/shader/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {

    const uint16_t width = 1280u;
    const uint16_t height = 720u;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    {
        leper::Renderer renderer{};

        auto test_mesh = leper::load_obj_mesh("bunny.obj");
        if (!test_mesh.has_value()) {
            spdlog::error("Failed to load OBJ model");
            return -1;
        }

        leper::ECS ecs;
        ecs.register_component<leper::MeshComponent>();
        ecs.register_component<leper::TransformComponent>();
        ecs.register_component<leper::BasicMaterialComponent>();

        leper::TransformSystem transform_sys(&ecs);

        leper::Entity bunny = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(bunny, test_mesh.value());
        ecs.add_component<leper::TransformComponent>(bunny, {});

        auto mesh_from_ecs = ecs.get_component<leper::MeshComponent>(bunny);

        renderer.upload_mesh(mesh_from_ecs);
        renderer.create_shader<leper::BasicMaterial>();

        transform_sys.translate(bunny, {0.0f, -0.75f, 0.0f});

        float_t aspect = static_cast<float_t>(width) / static_cast<float>(height);
        float_t ortho_height = 1.0f;
        float_t ortho_width = ortho_height * aspect;

        glm::mat4 projection = glm::ortho(-ortho_width, ortho_width,
                                          -ortho_height, ortho_height,
                                          0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 sun_dir = glm::vec3{0.5f, 1.0f, 0.3f};

        while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers(window);
            glfwPollEvents();

            transform_sys.rotate_euler(bunny, {0.0f, 0.01f, 0.0f});
            transform_sys.update();

            renderer.start_frame();

            const glm::mat4 model = ecs.get_component<leper::TransformComponent>(bunny).model;
            leper::Shader* basic_shader = renderer.get_material_shader<leper::BasicMaterial>();
            basic_shader->bind();
            basic_shader->set_uniform_vec3f("sunDir", sun_dir);
            basic_shader->set_uniform_mat4f("model", model);
            basic_shader->set_uniform_mat4f("view", view);
            basic_shader->set_uniform_mat4f("projection", projection);

            renderer.draw_mesh(mesh_from_ecs);

            renderer.finish_frame(width, height);
        }
    }

    glfwTerminate();
    return 0;
}
