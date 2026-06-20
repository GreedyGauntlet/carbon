#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "data/declarations.h"

struct Application {
    const char* name;
    const char* goodbye;
    ARRLIST_ScenePtr scenes;
    size_t current;
    HASHMAP_KeyMap keymap;
    ARRLIST_int keylist;

    #ifndef PROD_BUILD
    size_t memory;
    #endif
};

struct Scene {
    const char* name;
    ARRLIST_WorldPtr worlds;
};

struct World {
    WorldDrawFunction draw;
    WorldUpdateFunction update;
    WorldKeyEventFunction key;
    WorldMouseButtonEventFunction mousebutton;
    WorldMouseScrollEventFunction mousescroll;
    WorldMouseMoveFunction mousemove;
    WorldCleanFunction clean;
    Registry* registry;
    ARRLIST_SystemPtr systems;
};

struct Entity {
    EntityID id;
    World* context;
};

struct System {
    SystemDrawFunction draw;
    SystemUpdateFunction update;
    SystemKeyEventFunction key;
    SystemMouseButtonEventFunction mousebutton;
    SystemMouseScrollEventFunction mousescroll;
    SystemMouseMoveFunction mousemove;
    SystemCleanFunction clean;
    World* context;
};

struct ComponentStorage {
    ARRLIST_voidPtr dense;
    ARRLIST_EntityID entities;
    HASHMAP_EntityMap sparse;
};

struct Registry {
    EntityID next;
    HASHMAP_StorageMap storage;
    ARRLIST_ComponentStoragePtr dense;
    ARRLIST_size_t types;
};

#endif
