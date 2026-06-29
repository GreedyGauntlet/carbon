#include "world.h"
#include "data/definitions.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "ecs/registry.h"
#include "systems/system.h"
#include "systems/draw.h"
#include "systems/family.h"

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
    AddSystem(world, GenerateDrawSystem());
    AddSystem(world, GenerateFamilySystem());
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
    ARRLIST_EntityID* scripts = GetEntities(world, ScriptComponent);
    if (scripts) {
        for (size_t i = 0; i < scripts->size; i++) {
            Entity e = (Entity){ scripts->data[i], world };
            ScriptComponent* sc = GetComponent(e, ScriptComponent);
            if (sc->clean) sc->clean(e);
        }
    }
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

void DestroyEntity(Entity e) {
    ARRLIST_EntityID_add(&(e.context->removal), e.id);
    if (HasComponent(e, ScriptComponent)) {
        ScriptComponent* sc = GetComponent(e, ScriptComponent);
        if (sc->clean) sc->clean(e);
    }
}

void FlushRemovalQueue(World* world) {
    for (size_t i = 0; i < world->removal.size; i++)
        RegistryEraseEntity(world->registry, world->removal.data[i]);
    ARRLIST_EntityID_clear(&(world->removal));
}
