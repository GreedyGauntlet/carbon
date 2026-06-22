#ifndef ENTITY_H
#define ENTITY_H

#include "data/declarations.h"

#define AddComponent(entity, component, ...) ((component*)EntityAddComponent(entity, (size_t)component##_TYPE, &(component){ __VA_ARGS__ }, sizeof(component)))
#define GetComponent(entity, component) ((component*)EntityGetComponent(entity, (size_t)component##_TYPE))
#define HasComponent(entity, component) EntityHasComponent(entity, (size_t)component##_TYPE)
#define RemoveComponent(entity, component) EntityRemoveComponent(entity, (size_t)component##_TYPE)

struct Entity {
    EntityID id;
    World* context;
};

void* EntityAddComponent(const Entity e, size_t type, void* component, size_t size);

void* EntityGetComponent(const Entity e, size_t type);

BOOL EntityHasComponent(const Entity e, size_t type);

void EntityRemoveComponent(const Entity e, size_t type);

Vector3* EntityPosition(const Entity e);

Vector2* EntityScale(const Entity e);

float* EntityRotation(const Entity e);

#endif
