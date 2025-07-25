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

// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     glViewport(0, 0, width, height);
// }

int main() {

    const uint16_t width = 1280u;
    const uint16_t height = 720u;

    float_t aspect = static_cast<float_t>(width) / static_cast<float>(height);
    float_t ortho_height = 1.0f;
    float_t ortho_width = ortho_height * aspect;

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
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    {
        auto bunny_mesh = leper::load_obj_mesh("bunny.obj");
        auto floor_mesh = leper::load_obj_mesh("floor.obj");
        if (!bunny_mesh.has_value()) {
            spdlog::error("Failed to load OBJ models");
            return -1;
        }

        leper::Renderer renderer{};

        leper::ECS ecs;
        ecs.register_component<leper::MeshComponent>();
        ecs.register_component<leper::TransformComponent>();
        ecs.register_component<leper::BasicMaterialComponent>();
        ecs.register_component<leper::CameraComponent>();

        leper::TransformSystem transform_sys(&ecs);
        leper::RenderingSystem rendering_sys(&ecs, &renderer);

        leper::Entity camera = ecs.create_entity();
        ecs.add_component<leper::CameraComponent>(camera, {
                                                              .view = glm::lookAt(glm::vec3(0.0f, 1.0f, 3.0f),
                                                                                  glm::vec3(0.0f, 0.2f, 0.0f),
                                                                                  glm::vec3(0.0f, 1.0f, 0.0f)),
                                                              .projection = glm::ortho(-ortho_width, ortho_width,
                                                                                       -ortho_height, ortho_height,
                                                                                       0.1f, 100.0f),
                                                          });

        leper::Entity bunny = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(bunny, bunny_mesh.value());
        ecs.add_component<leper::TransformComponent>(bunny, {});
        ecs.add_component<leper::BasicMaterialComponent>(bunny, {.albedo = {0.831f, 0.624f, 0.329f}});

        leper::Entity floor = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(floor, floor_mesh.value());
        ecs.add_component<leper::TransformComponent>(floor, {});
        ecs.add_component<leper::BasicMaterialComponent>(floor, {.albedo = {0.549f, 0.82f, 0.447f}});

        transform_sys.scale(bunny, {0.5f, 0.5f, 0.5f});
        transform_sys.translate(bunny, {0.0f, 0.0f, 0.0f});

        transform_sys.scale(floor, {5.0f, 5.0f, 5.0f});

        while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers(window);
            glfwPollEvents();

            transform_sys.rotate_euler(bunny, {0.0f, 0.01f, 0.0f});
            transform_sys.update();

            int fb_width, fb_height;
            glfwGetFramebufferSize(window, &fb_width, &fb_height);

            rendering_sys.draw(fb_width, fb_height, camera);
        }
    }

    glfwTerminate();
    return 0;
}
