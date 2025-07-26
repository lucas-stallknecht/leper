#include "rendering_system.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "leper/leper_ecs_components.h"
#include "leper/leper_ecs_types.h"
#include "leper/leper_rendering_constants.h"

constexpr glm::vec3 dummy_point_pos = {0.4f, 0.3f, 0.6f};
constexpr glm::vec3 dummy_point_color = {0.0f, 0.0f, 1.0f};

namespace leper {

    RenderingSystem::RenderingSystem(ECS* ecs, Renderer* renderer) : ecs_(ecs), renderer_(renderer) {
        assert(ecs_ && renderer_ && "ECS or Renderer is not set correctly");

        renderer_->create_shader<BasicMaterial>();
    }

    void RenderingSystem::draw(uint16_t width, uint16_t height, Entity camera) {
        renderer_->start_frame();

        CameraComponent camera_data = ecs_->get_component<CameraComponent>(camera);

        // --- Basic Material ---

        Shader* basic_shader = renderer_->get_material_shader<BasicMaterial>();
        basic_shader->bind();

        basic_shader->set_uniform_mat4f("projection", camera_data.projection);
        basic_shader->set_uniform_mat4f("view", camera_data.view);

        // --- Lights ---

        // Acessing data directly should not be allowed
        // This is temporary: use a Scene instead but this will do since I know the number of lights
        ComponentArray<DirectionalLightComponent>* dir_lights_array = ecs_->get_component_array<DirectionalLightComponent>();
        if (dir_lights_array->data().size()) {
            const DirectionalLightComponent dir_light = dir_lights_array->data()[0];

            basic_shader->set_uniform_vec3f("dirLight.dir", dir_light.direction);
            basic_shader->set_uniform_1f("dirLight.intensity", dir_light.intensity);
            basic_shader->set_uniform_vec3f("dirLight.col", dir_light.color);
        }

        auto point_entities = ecs_->get_entities_with_components<PointLightComponent, TransformComponent>();
        const size_t min_point_lights = std::min(point_entities.size(), MAX_POINT_LIGHTS);

        for (size_t i = 0; i < min_point_lights; i++) {
            const Entity entity = point_entities[i];
            const TransformComponent transform = ecs_->get_component<TransformComponent>(entity);

            basic_shader->set_uniform_vec3f("pointLights[" + std::to_string(i) + "].pos", transform.transform.position);

            const PointLightComponent point_comp = ecs_->get_component<PointLightComponent>(entity);
            basic_shader->set_uniform_vec3f("pointLights[" + std::to_string(i) + "].col", point_comp.color);
            basic_shader->set_uniform_1f("pointLights[" + std::to_string(i) + "].intensity", point_comp.intensity);
        }

        // --- Meshes with BasicMaterial ---

        auto mesh_entities = ecs_->get_entities_with_components<MeshComponent, BasicMaterialComponent, TransformComponent>();

        ComponentArray<MeshComponent>* meshes_array = ecs_->get_component_array<MeshComponent>();
        for (auto entity : mesh_entities) {
            const MeshComponent mesh = ecs_->get_component<MeshComponent>(entity);

            if (!renderer_->has_mesh_objects(mesh))
                renderer_->upload_mesh(mesh);

            const glm::mat4 model = ecs_->get_component<TransformComponent>(entity).model;
            basic_shader->set_uniform_mat4f("model", model);
            const glm::vec3 albedo = ecs_->get_component<BasicMaterialComponent>(entity).albedo;
            basic_shader->set_uniform_vec3f("u_color", albedo);

            renderer_->draw_mesh(mesh);
        }

        renderer_->finish_frame(width, height);
    }

} // namespace leper
