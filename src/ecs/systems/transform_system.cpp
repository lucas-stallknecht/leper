#include "transform_system.h"

#include <glm/ext/matrix_transform.hpp>

#include "leper/leper_ecs_components.h"

namespace leper {

    TransformSystem::TransformSystem(ECS* ecs) : ecs_(ecs) {
    }

    void TransformSystem::update() {
        auto& all_transform_components = ecs_->get_component_array<TransformComponent>()->data();
        for (auto& comp : all_transform_components) {
            if (comp.is_dirty) {
                glm::mat4 new_model = glm::identity<glm::mat4>();
                new_model = glm::scale(new_model, comp.transform.scale);
                new_model *= glm::mat4(comp.transform.rotation);
                new_model = glm::translate(new_model, comp.transform.position);
                comp.model = new_model;
                comp.is_dirty = false;
            }
        }
    }

    void TransformSystem::translate(Entity entity, const glm::vec3& delta) {
        if (ecs_->has_component<TransformComponent>(entity)) {
            auto& transform_comp = ecs_->get_component<TransformComponent>(entity);
            transform_comp.transform.position += delta;
            transform_comp.is_dirty = true;
        }
    }

    void TransformSystem::scale(Entity entity, const glm::vec3& factor) {
        if (ecs_->has_component<TransformComponent>(entity)) {
            auto& transform_comp = ecs_->get_component<TransformComponent>(entity);
            transform_comp.transform.scale *= factor;
            transform_comp.is_dirty = true;
        }
    }

    void TransformSystem::rotate(Entity entity, const glm::quat& delta_rotation) {
        if (ecs_->has_component<TransformComponent>(entity)) {
            auto& transform_comp = ecs_->get_component<TransformComponent>(entity);
            transform_comp.transform.rotation = delta_rotation * transform_comp.transform.rotation;
            transform_comp.is_dirty = true;
        }
    }

    void TransformSystem::rotate_euler(Entity entity, const glm::vec3& euler_radians) {
        glm::quat delta_quat = glm::quat(euler_radians);
        rotate(entity, delta_quat);
    }

} // namespace leper
