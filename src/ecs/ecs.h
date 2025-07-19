#pragma once

#include <cstdint>
#include <array>
#include <queue>
#include <unordered_map>
#include <cassert>
#include <typeindex>

#include "leper/leper_ecs_types.h"
#include "component_array.h"

namespace leper {

    class ECS {
      public:
        ECS() {
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
                available_entities_.push(entity);
            }
        }

        Entity create_entity();
        void destroy_entity(Entity entity);

        template <typename T>
        void register_component() {
            std::type_index type_name = typeid(T);
            assert(component_arrays_.find(type_name) == component_arrays_.end() && "Component already registered");

            component_arrays_[type_name] = new ComponentArray<T>();

            Signature sig;
            sig.set(component_type_signatures_.size());
            component_type_signatures_[type_name] = sig;
        }
        // NOTE: We can't unregister components. Is is intended :)

        template <typename T>
        void add_component(Entity entity, T component) {
            std::type_index type_name = typeid(T);
            auto it = component_arrays_.find(type_name);
            assert(it != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(it->second);
            comp_arr->insert_data(entity, component);

            signatures_[entity] |= component_type_signatures_.at(type_name);
        }

        template <typename T>
        void remove_component(Entity entity) {
            std::type_index type_name = typeid(T);
            auto it = component_arrays_.find(type_name);
            assert(it != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(it->second);
            comp_arr->remove_data(entity);

            signatures_[entity] &= ~component_type_signatures_.at(type_name);
        }

        template <typename T>
        T& get_component(Entity entity) {
            std::type_index type_name = typeid(T);
            auto it = component_arrays_.find(type_name);
            assert(it != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(it->second);
            return comp_arr->get(entity);
        }

        void clear();

      private:
        std::queue<Entity> available_entities_;
        std::array<Signature, MAX_ENTITIES> signatures_;
        uint32_t living_entities_ = 0;

        std::unordered_map<std::type_index, IComponentArray*> component_arrays_;
        std::unordered_map<std::type_index, Signature> component_type_signatures_;
    };

} // namespace leper
