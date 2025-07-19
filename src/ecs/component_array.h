#pragma once

#include <array>
#include <cstddef>
#include <unordered_map>
#include <cassert>

#include "leper/leper_ecs_types.h"

namespace leper {
    // Used for typing only
    class IComponentArray {
      public:
        virtual ~IComponentArray() = default;
    };

    template <typename T>
    class ComponentArray : public IComponentArray {
      public:
        void insert_data(Entity entity, T component) {
            assert(entity_to_index_map_.find(entity) == entity_to_index_map_.end() && "Component added to same entity more than once");

            size_t new_index = size_;
            entity_to_index_map_[entity] = new_index;
            index_to_entity_map_[new_index] = entity;
            data_[new_index] = component;

            size_++;
        }

        void remove_data(Entity entity) {
            assert(entity_to_index_map_.find(entity) != entity_to_index_map_.end() && "Removing non-existant component data");

            size_t index_of_removed = entity_to_index_map_[entity];
            size_t index_of_last_component = size_ - 1;
            data_[index_of_removed] = data_[index_of_last_component];

            if (index_of_removed != index_of_last_component) {
                // Move last component into the place of the removed one
                data_[index_of_removed] = data_[index_of_last_component];

                Entity entity_of_moved = index_to_entity_map_[index_of_last_component];
                entity_to_index_map_[entity_of_moved] = index_of_removed;
                index_to_entity_map_[index_of_removed] = entity_of_moved;
            }

            entity_to_index_map_.erase(entity);
            index_to_entity_map_.erase(index_of_last_component); // not index_of_removed

            size_--;
        }

        T& get(Entity entity) {
            assert(entity_to_index_map_.find(entity) != entity_to_index_map_.end() && "Retrieving non-existant component data");
            return data_[entity_to_index_map_[entity]];
        }

      private:
        // Packed array of components
        std::array<T, MAX_ENTITIES> data_;

        std::unordered_map<Entity, size_t> entity_to_index_map_;
        std::unordered_map<size_t, Entity> index_to_entity_map_;

        size_t size_ = 0;
    };

}; // namespace leper
