#pragma once

#include <cstdint>
#include <queue>
#include <set>
#include <unordered_map>
#include <cassert>
#include <typeindex>

#include "leper/leper_ecs_types.h"
#include "component_array.h"

namespace leper {

    // --- Component IDs utils
    static size_t component_id_counter = 0;

    template <typename T>
    size_t get_component_id() {
        static size_t id = component_id_counter++;
        return id;
    }
    // ---

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

            Signature new_sig = entity_signatures_[entity];
            size_t comp_id = get_component_id<T>();
            new_sig.set(comp_id);
            update_entity_signature(entity, new_sig);
        }

        template <typename T>
        void remove_component(Entity entity) {
            std::type_index type_id = typeid(T);
            auto target_array = component_arrays_.find(type_id);
            assert(target_array != component_arrays_.end() && "Component not registered");

            auto* comp_arr = static_cast<ComponentArray<T>*>(target_array->second);
            comp_arr->remove(entity);

            Signature new_sig = entity_signatures_[entity];
            size_t comp_id = get_component_id<T>();
            new_sig.reset(comp_id);
            update_entity_signature(entity, new_sig);
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

        template <typename... Components>
        std::vector<Entity> get_entities_with_components() const {
            Signature sig;
            (sig.set(get_component_id<Components>()), ...);

            return query_entities_with_signature(sig);
        }

      private:
        void update_entity_signature(Entity entity, const Signature& new_signature);
        std::vector<Entity> query_entities_with_signature(const Signature& required) const;

        std::queue<Entity> available_entities_;
        uint32_t active_entity_count_ = 0;

        std::unordered_map<std::type_index, IComponentArray*> component_arrays_;

        std::array<Signature, MAX_ENTITIES> entity_signatures_;
        std::unordered_map<Signature, std::set<Entity>> signature_to_entities_;
    };

} // namespace leper
