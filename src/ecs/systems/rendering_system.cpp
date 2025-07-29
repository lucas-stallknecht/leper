#include "rendering_system.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "leper/leper_ecs_components.h"
#include "leper/leper_ecs_types.h"
#include "leper/leper_rendering_constants.h"

glm::vec3 srgb_to_linear(glm::vec3 c) {
    return glm::vec3(
        pow(c.r, 2.2f),
        pow(c.g, 2.2f),
        pow(c.b, 2.2f));
}

namespace leper {

    RenderingSystem::RenderingSystem(ECS* ecs, Renderer* renderer) : ecs_(ecs), renderer_(renderer) {
        assert(ecs_ && renderer_ && "ECS or Renderer is not set correctly");

        renderer_->create_shader<ToonMaterial>();
    }

    void RenderingSystem::draw_shadow_map_(const glm::mat4& light_matrix) {
        renderer_->start_shadow_frame();

        Shader* depth_shader = renderer_->get_depth_shader();
        depth_shader->bind();

        depth_shader->set_uniform_mat4f("lightMatrix", light_matrix);

        auto mesh_entities = ecs_->get_entities_with_components<MeshComponent, ToonMaterial, TransformComponent>();
        ComponentArray<MeshComponent>* meshes_array = ecs_->get_component_array<MeshComponent>();

        for (auto entity : mesh_entities) {
            const MeshComponent mesh = ecs_->get_component<MeshComponent>(entity);

            if (!renderer_->has_mesh_objects(mesh))
                renderer_->upload_mesh(mesh);

            const glm::mat4 model = ecs_->get_component<TransformComponent>(entity).model;
            depth_shader->set_uniform_mat4f("model", model);

            renderer_->draw_mesh(mesh);
        }
    }

    void RenderingSystem::draw(uint16_t width, uint16_t height, Entity camera) {

        ComponentArray<DirectionalLightComponent>* dir_lights_array = ecs_->get_component_array<DirectionalLightComponent>();
        glm::mat4 light_matrix = glm::identity<glm::mat4>();
        if (dir_lights_array->data().size()) {
            const DirectionalLightComponent dir_light = dir_lights_array->data()[0];

            glm::vec3 light_pos = dir_light.direction * 10.0f;
            glm::mat4 light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
            glm::mat4 light_view = glm::lookAt(light_pos,
                                               glm::vec3(0.0f, 0.0f, 0.0f),
                                               glm::vec3(0.0f, 1.0f, 0.0f));
            light_matrix = light_proj * light_view;
        }

        draw_shadow_map_(light_matrix);

        renderer_->start_main_frame();
        CameraComponent camera_data = ecs_->get_component<CameraComponent>(camera);

        // --- Toon Material ---

        Shader* toon_shader = renderer_->get_material_shader<ToonMaterial>();
        toon_shader->bind();

        toon_shader->set_uniform_mat4f("projection", camera_data.projection);
        toon_shader->set_uniform_mat4f("view", camera_data.view);
        toon_shader->set_uniform_vec3f("viewPos", glm::vec3(0.0f, 2.0f, 3.0f));

        // --- Lights ---

        // Acessing data directly should not be allowed
        // This is temporary: use a Scene instead but this will do since I know the number of lights
        if (dir_lights_array->data().size()) {
            const DirectionalLightComponent dir_light = dir_lights_array->data()[0];

            toon_shader->set_uniform_vec3f("dirLight.dir", dir_light.direction);
            toon_shader->set_uniform_1f("dirLight.intensity", dir_light.intensity);
            toon_shader->set_uniform_vec3f("dirLight.col", dir_light.color);
            toon_shader->set_uniform_mat4f("lightMatrix", light_matrix);
        }

        auto point_entities = ecs_->get_entities_with_components<PointLightComponent, TransformComponent>();
        const size_t min_point_lights = std::min(point_entities.size(), MAX_POINT_LIGHTS);

        for (size_t i = 0; i < min_point_lights; i++) {
            const Entity entity = point_entities[i];
            const TransformComponent transform = ecs_->get_component<TransformComponent>(entity);

            toon_shader->set_uniform_vec3f("pointLights[" + std::to_string(i) + "].pos", transform.transform.position);

            const PointLightComponent point_comp = ecs_->get_component<PointLightComponent>(entity);
            toon_shader->set_uniform_vec3f("pointLights[" + std::to_string(i) + "].col", point_comp.color);
            toon_shader->set_uniform_1f("pointLights[" + std::to_string(i) + "].intensity", point_comp.intensity);
        }

        // --- Meshes with ToonMaterial ---

        auto mesh_entities = ecs_->get_entities_with_components<MeshComponent, ToonMaterial, TransformComponent>();
        ComponentArray<MeshComponent>* meshes_array = ecs_->get_component_array<MeshComponent>();

        for (auto entity : mesh_entities) {
            const MeshComponent mesh = ecs_->get_component<MeshComponent>(entity);

            if (!renderer_->has_mesh_objects(mesh))
                renderer_->upload_mesh(mesh);

            const glm::mat4 model = ecs_->get_component<TransformComponent>(entity).model;
            toon_shader->set_uniform_mat4f("model", model);
            const glm::vec3 albedo = ecs_->get_component<ToonMaterial>(entity).albedo;
            toon_shader->set_uniform_vec3f("u_color", srgb_to_linear(albedo));

            renderer_->draw_mesh(mesh);
        }

        renderer_->finish_main_frame(width, height);
    }

} // namespace leper
