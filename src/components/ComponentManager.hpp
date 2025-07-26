#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include "src/components/Components.hpp"

#include <entt/entt.hpp>

class ComponentManager {
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

    static entt::registry& getRegistry() { return registry; }

private:
    static entt::registry registry;
};

#endif // COMPONENT_MANAGER_HPP
