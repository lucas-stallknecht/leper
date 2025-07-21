#pragma once

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <cassert>
#include <typeindex>

#include "leper/leper_ecs_types.h"
#include "component_array.h"

namespace leper {

    class ECS {
      public:
        ECS();
        ~ECS();

        Entity create_entity();
        void destroy_entity(Entity entity);

        template <typename T>
        void register_component() {
            std::type_index type_id = typeid(T);
            assert(component_arrays_.find(type_id) == component_arrays_.end() && "Component already registered");

            component_arrays_[type_id] = new ComponentArray<T>();
        }
        // NOTE: We can't unregister components. Is is intended :)

        template <typename T>
        void add_component(Entity entity, T component) {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(target_array->second);
            comp_arr->insert(entity, component);
        }

        template <typename T>
        void remove_component(Entity entity) {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(target_array->second);
            comp_arr->remove(entity);
        }

        template <typename T>
        bool has_component(Entity entity) const {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(target_array->second);
            return comp_arr->has(entity);
        }

        template <typename T>
        T& get_component(Entity entity) {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(target_array->second);
            return comp_arr->get(entity);
        }

        template <typename T>
        ComponentArray<T>* get_component_array() const {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            return static_cast<ComponentArray<T>*>(component_arrays_.at(type_id));
        }

      private:
        std::queue<Entity> available_entities_;
        uint32_t active_entity_count_ = 0;

        std::unordered_map<std::type_index, IComponentArray*> component_arrays_;
    };

} // namespace leper
