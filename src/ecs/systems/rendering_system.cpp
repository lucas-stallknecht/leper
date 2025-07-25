#include "rendering_system.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "leper/leper_ecs_components.h"
#include "leper/leper_ecs_types.h"

constexpr glm::vec3 dummy_sun_dir = glm::vec3{0.5f, 1.0f, 0.3f};

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
        basic_shader->set_uniform_vec3f("sunDir", dummy_sun_dir);
        basic_shader->set_uniform_mat4f("projection", camera_data.projection);
        basic_shader->set_uniform_mat4f("view", camera_data.view);

        auto entity_vector = ecs_->get_entities_with_components<MeshComponent, BasicMaterialComponent, TransformComponent>();

        ComponentArray<MeshComponent>* meshes_array = ecs_->get_component_array<MeshComponent>();
        for (auto entity : entity_vector) {
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
