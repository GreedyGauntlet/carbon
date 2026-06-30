#ifndef WORLD_H
#define WORLD_H

#include "data/declarations.h"

#define GetEntities(world, component) WorldGetEntities(world, (size_t)component##_TYPE)

World* GenerateWorld(
        const char* name,
        WorldDrawFunction draw,
        WorldUpdateFunction update,
        WorldKeyEventFunction key,
        WorldMouseButtonEventFunction mousebutton,
        WorldMouseScrollEventFunction mousescroll,
        WorldMouseMoveFunction mousemove,
        WorldCleanFunction clean);

Entity CreateEntity(World* world);

Entity CreateEntityP(World* world, float x, float y, float z);

Entity CreateEntityN(World* world, const char* name);

Entity CreateEntityNP(World* world, const char* name, float x, float y, float z);

void AddSystem(World* world, System* system);

void DestroyWorld(World* world);

ARRLIST_EntityID* WorldGetEntities(World* world, size_t type);

void DestroyEntity(Entity e);

void FlushRemovalQueue(World* world);

BOOL HasParent(Entity e);

BOOL HasChild(Entity parent, Entity child);

void OrphanChild(Entity e);

Entity GetParent(Entity e);

ARRLIST_EntityID* GetChildren(Entity e);

void LinkFamily(Entity parent, Entity child);

Vector3 GetWorldPosition(Entity e);

Vector2 GetWorldScale(Entity e);

float GetWorldRotation(Entity e);

#endif
