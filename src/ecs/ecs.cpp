#include "ecs.h"

#include <cassert>

namespace leper {

    ECS::ECS() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            available_entities_.push(entity);
        }
    }

    Entity ECS::create_entity() {
        assert(active_entity_count_ < MAX_ENTITIES && "Too many living entities");

        Entity id = available_entities_.front();
        available_entities_.pop();
        active_entity_count_++;

        return id;
    }

    void ECS::destroy_entity(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range");

        available_entities_.push(entity);
        active_entity_count_--;
    }

    ECS::~ECS() {
        for (auto& [type, ptr] : component_arrays_) {
            delete ptr; // since only the ECS owns it
        }
        component_arrays_.clear();
    }

} // namespace leper
