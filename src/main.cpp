#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "asset_loading/obj_loading.h"
#include "ecs/ecs.h"
#include "ecs/systems/rendering_system.h"
#include "ecs/systems/transform_system.h"
#include "leper/leper_common_types.h"
#include "leper/leper_ecs_components.h"
#include "leper/leper_ecs_types.h"
#include "renderer/renderer.h"

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
        auto test_mesh = leper::load_obj_mesh("bunny.obj");
        if (!test_mesh.has_value()) {
            spdlog::error("Failed to load OBJ model");
            return -1;
        }

        leper::Renderer renderer{};

        leper::ECS ecs;
        ecs.register_component<leper::MeshComponent>();
        ecs.register_component<leper::TransformComponent>();
        ecs.register_component<leper::BasicMaterialComponent>();

        leper::TransformSystem transform_sys(&ecs);
        leper::RenderingSystem rendering_sys(&ecs, &renderer);

        leper::Entity bunny = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(bunny, test_mesh.value());
        ecs.add_component<leper::TransformComponent>(bunny, {});
        ecs.add_component<leper::BasicMaterialComponent>(bunny, {});

        transform_sys.translate(bunny, {0.0f, -0.75f, 0.0f});

        while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers(window);
            glfwPollEvents();

            transform_sys.rotate_euler(bunny, {0.0f, 0.01f, 0.0f});
            transform_sys.update();
            rendering_sys.draw(width, height);
        }
    }

    glfwTerminate();
    return 0;
}
