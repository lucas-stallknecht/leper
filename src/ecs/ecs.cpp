#include "ecs.h"

#include <cassert>

namespace leper {

    Entity ECS::create_entity() {
        assert(living_entities_ < MAX_ENTITIES && "Too many living entities");

        Entity id = available_entities_.front();
        available_entities_.pop();
        living_entities_++;

        return id;
    }

    void ECS::destroy_entity(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range");

        signatures_[entity].reset();

        available_entities_.push(entity);
        living_entities_--;
    }

    void ECS::clear() {
        for (auto& [type, ptr] : component_arrays_) {
            delete ptr; // since only the ECS owns it
        }
        component_arrays_.clear();
        component_type_signatures_.clear();
    }

} // namespace leper
