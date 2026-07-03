#include "entity.h"
#include "data/definitions.h"
#include "ecs/components.h"
#include "ecs/registry.h"

void* EntityAddComponent(const Entity e, size_t type, void* component, size_t size) {
    return RegistryEmplaceComponent(e.context->registry, e.id, type, component, size);
}

void* EntityGetComponent(const Entity e, size_t type) {
    return RegistryGetComponent(e.context->registry, e.id, type);
}

BOOL EntityHasComponent(const Entity e, size_t type) {
    return RegistryHasComponent(e.context->registry, e.id, type);
}

void EntityRemoveComponent(const Entity e, size_t type) {
    RegistryRemoveComponent(e.context->registry, e.id, type);
}

Vector3* EntityPosition(const Entity e) {
    EZ_ASSERT(HasComponent(e, TransformComponent), "Entity does not have transform component!");
    TransformComponent* tc = GetComponent(e, TransformComponent);
    return &(tc->translation);
}

Vector2* EntityScale(const Entity e) {
    EZ_ASSERT(HasComponent(e, TransformComponent), "Entity does not have transform component!");
    TransformComponent* tc = GetComponent(e, TransformComponent);
    return &(tc->scale);
}

float* EntityRotation(const Entity e) {
    EZ_ASSERT(HasComponent(e, TransformComponent), "Entity does not have transform component!");
    TransformComponent* tc = GetComponent(e, TransformComponent);
    return &(tc->rotation);
}

Script* EntityScript(const Entity e) {
    if (HasComponent(e, ScriptComponent)) {
        Scene* scene = e.context->parent;
        size_t scriptid = GetComponent(e, ScriptComponent)->id;
        if (scriptid == (size_t)-1) return NULL;
        EZ_ASSERT(scriptid < scene->scripts.scripts.size, "Invalid script ID [%d] detected", (int)scriptid);
        return &(scene->scripts.scripts.data[scriptid]);
    }
    return NULL;
}
