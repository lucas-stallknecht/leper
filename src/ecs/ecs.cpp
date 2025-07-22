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

    void ECS::update_entity_signature(Entity entity, const Signature& new_signature) {
        Signature old_signature = entity_signatures_[entity];

        // Remove from old signature group
        auto it = signature_to_entities_.find(old_signature);
        if (it != signature_to_entities_.end()) {
            it->second.erase(entity);

            // No more entity left with these components
            if (it->second.empty()) {
                signature_to_entities_.erase(it);
            }
        }

        // Set new signature
        entity_signatures_[entity] = new_signature;
        signature_to_entities_[new_signature].insert(entity);
    }

    std::vector<Entity> ECS::query_entities_with_signature(const Signature& required) const {
        std::vector<Entity> result;
        for (const auto& [sig, entities] : signature_to_entities_) {
            if ((sig & required) == required) {
                result.insert(result.end(), entities.begin(), entities.end());
            }
        }
        return result;
    }

    ECS::~ECS() {
        for (auto& [type, ptr] : component_arrays_) {
            delete ptr; // since only the ECS owns it
        }
        component_arrays_.clear();
    }

} // namespace leper
