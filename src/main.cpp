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

    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    });

    GLFWwindow* window = glfwCreateWindow(width, height, "Leper", nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    {
        auto sphere_mesh = leper::load_obj_mesh("sphere.obj");
        auto floor_mesh = leper::load_obj_mesh("floor.obj");
        if (!sphere_mesh.has_value()) {
            spdlog::error("Failed to load OBJ models");
            return -1;
        }

        leper::Renderer renderer{};

        leper::ECS ecs;
        ecs.register_component<leper::MeshComponent>();
        ecs.register_component<leper::TransformComponent>();
        ecs.register_component<leper::ToonMaterial>();
        ecs.register_component<leper::CameraComponent>();
        ecs.register_component<leper::DirectionalLightComponent>();
        ecs.register_component<leper::PointLightComponent>();

        leper::TransformSystem transform_sys(&ecs);
        leper::RenderingSystem rendering_sys(&ecs, &renderer);

        leper::Entity camera = ecs.create_entity();
        ecs.add_component<leper::CameraComponent>(camera, {
                                                              .view = glm::lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
                                                                                  glm::vec3(0.0f, 0.2f, 0.0f),
                                                                                  glm::vec3(0.0f, 1.0f, 0.0f)),
                                                              .projection = glm::ortho(-ortho_width, ortho_width,
                                                                                       -ortho_height, ortho_height,
                                                                                       0.1f, 100.0f),
                                                          });

        leper::Entity sphere = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(sphere, sphere_mesh.value());
        ecs.add_component<leper::TransformComponent>(sphere, {});
        ecs.add_component<leper::ToonMaterial>(sphere, {.albedo = {0.28f, 0.6f, 0.96f}});

        leper::Entity floor = ecs.create_entity();
        ecs.add_component<leper::MeshComponent>(floor, floor_mesh.value());
        ecs.add_component<leper::TransformComponent>(floor, {});
        ecs.add_component<leper::ToonMaterial>(floor, {.albedo = {0.25f, 0.25f, 0.25f}});

        transform_sys.scale(sphere, {0.3f, 0.3f, 0.3f});
        transform_sys.translate(sphere, {0.0f, 1.0f, 0.0f});

        transform_sys.scale(floor, {5.0f, 5.0f, 5.0f});

        // Lights
        leper::Entity sun = ecs.create_entity();
        ecs.add_component<leper::DirectionalLightComponent>(sun, {.color = {1.0f, 0.95f, 0.9f}, .intensity = 0.6f, .direction = {0.4f, 1.0f, 0.1f}});

        leper::Entity point_red = ecs.create_entity();
        ecs.add_component<leper::TransformComponent>(point_red, {});
        ecs.add_component<leper::PointLightComponent>(point_red, {.color = {1.0f, 0.0f, 0.0f}, .intensity = 1.0f});

        leper::Entity point_green = ecs.create_entity();
        ecs.add_component<leper::TransformComponent>(point_green, {});
        ecs.add_component<leper::PointLightComponent>(point_green, {.color = {0.0f, 1.0f, 0.0f}, .intensity = 1.0f});

        leper::Entity point_blue = ecs.create_entity();
        ecs.add_component<leper::TransformComponent>(point_blue, {});
        ecs.add_component<leper::PointLightComponent>(point_blue, {.color = {0.0f, 0.0f, 1.0f}, .intensity = 1.0f});

        const float_t rot_radius = 1.25f;
        const float_t rot_speed = 0.01f;
        float_t theta = 0.0f;

        while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers(window);
            glfwPollEvents();

            auto& red_t = ecs.get_component<leper::TransformComponent>(point_red);
            red_t.transform.position = {rot_radius * cos(theta), 1.0f, rot_radius * sin(theta)};

            auto& green_t = ecs.get_component<leper::TransformComponent>(point_blue);
            green_t.transform.position = {rot_radius * cos(theta + 2.095f), 1.0f, rot_radius * sin(theta + 2.095f)};

            auto& blue_t = ecs.get_component<leper::TransformComponent>(point_green);
            blue_t.transform.position = {rot_radius * cos(theta + 4.188f), 1.0f, rot_radius * sin(theta + 4.188f)};

            // transform_sys.rotate_euler(sphere, {0.0f, 0.01f, 0.0f});
            transform_sys.update();

            int fb_width, fb_height;
            glfwGetFramebufferSize(window, &fb_width, &fb_height);

            rendering_sys.draw(fb_width, fb_height, camera);

            transform_sys.translate(point_red, {0.0f, 1.0f, -1.25f});

            theta += rot_speed;
        }
    }

    glfwTerminate();
    return 0;
}
