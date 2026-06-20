#include "world.h"
#include "data/definitions.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "ecs/registry.h"
#include "systems/system.h"

World* GenerateWorld(
        WorldDrawFunction draw,
        WorldUpdateFunction update,
        WorldKeyEventFunction key,
        WorldMouseButtonEventFunction mousebutton,
        WorldMouseScrollEventFunction mousescroll,
        WorldMouseMoveFunction mousemove,
        WorldCleanFunction clean) {
    World* world = EZ_ALLOC(1, sizeof(World));
    world->draw = draw;
    world->update = update;
    world->key = key;
    world->mousebutton = mousebutton;
    world->mousescroll = mousescroll;
    world->mousemove = mousemove;
    world->clean = clean;
    world->registry = GenerateRegistry();
    return world;
}

Entity CreateEntity(World* world) {
    Entity e = (Entity){ RegistryCreateEntity(world->registry), world };
    AddComponent(e, TransformComponent, {0.0f, 0.0f, 0.0f}, 0.0f, {1.0f, 1.0f});
    return e;
}

Entity CreateEntityP(World* world, float x, float y, float z) {
    Entity e = CreateEntity(world);
    *(EntityPosition(e)) = (Vector3){ x, y, z };
    return e;
}

void AddSystem(World* world, System* system) {
    ARRLIST_SystemPtr_add(&world->systems, system);
    system->context = world;
}

void DestroyWorld(World* world) {
    if (world->clean) world->clean(world);
    for (size_t i = 0; i < world->systems.size; i++)
        DestroySystem(world->systems.data[i]);
    ARRLIST_SystemPtr_clear(&world->systems);
    DestroyRegistry(world->registry);
    EZ_FREE(world);
}

ARRLIST_EntityID* WorldGetEntities(World* world, size_t type) {
    return RegistryGetEntities(world->registry, type);
}
