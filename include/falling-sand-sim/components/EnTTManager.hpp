// EnTTManager.hpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// Provides a static interface for managing ECS entities and components using
// the EnTT library. Handles creation, destruction, and access to components.

#ifndef ENTT_MANAGER_HPP
#define ENTT_MANAGER_HPP

#include "falling-sand-sim/components/Components.hpp"

#include <entt/entt.hpp>

class EnTTManager {
public:
    static entt::entity createEntity() {
        return registry.create();
    }

    static void destroyEntity(entt::entity entity) {
        registry.destroy(entity);
    }

    template<typename Component, typename... Args>
    static void addComponent(entt::entity entity, Args&&... args) {
        registry.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template<typename Component>
    static void removeComponent(entt::entity entity) {
        registry.remove<Component>(entity);
    }

    template<typename Component>
    static bool hasComponent(entt::entity entity) {
        return registry.all_of<Component>(entity);
    }

    template<typename Component>
    static Component& getComponent(entt::entity entity) {
        return registry.get<Component>(entity);
    }

    template<typename Component>
    static Component* getComponentIfExists(const entt::entity entity) {
        if (registry.all_of<Component>(entity)) {
            return &registry.get<Component>(entity);
        }
        return nullptr;
    }

    static entt::registry& getRegistry() { return registry; }

private:
    static entt::registry registry;
};

#endif // ENTT_MANAGER_HPP
