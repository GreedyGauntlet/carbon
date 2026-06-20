#ifndef WORLD_H
#define WORLD_H

#include "data/declarations.h"

#define GetEntities(world, component) WorldGetEntities(world, (size_t)component##_TYPE)

World* GenerateWorld(
        WorldDrawFunction draw,
        WorldUpdateFunction update,
        WorldKeyEventFunction key,
        WorldMouseButtonEventFunction mousebutton,
        WorldMouseScrollEventFunction mousescroll,
        WorldMouseMoveFunction mousemove,
        WorldCleanFunction clean);

Entity CreateEntity(World* world);

Entity CreateEntityP(World* world, float x, float y, float z);

void AddSystem(World* world, System* system);

void DestroyWorld(World* world);

ARRLIST_EntityID* WorldGetEntities(World* world, size_t type);

#endif
