#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <raylib.h>
#include <stdint.h>
#include <easyobjects.h>
#include <easybasics.h>

#define DECLARE(name) struct name; typedef struct name name;

#define INVALID_ENTITY 0

typedef uint64_t EntityID;

typedef enum {
    INPUTPRESS,
    INPUTRELEASE,
    INPUTDOWN,
} InputAction;

DECLARE(Application);
DECLARE(Scene);
DECLARE(World);
DECLARE(Entity);
DECLARE(System);
DECLARE(ComponentStorage);
DECLARE(Registry);

typedef void (*WorldDrawFunction)(World* world);
typedef void (*WorldUpdateFunction)(World* world, float dt);
typedef void (*WorldKeyEventFunction)(World* world, int key, InputAction action);
typedef void (*WorldMouseButtonEventFunction)(World* world, int key, InputAction action);
typedef void (*WorldMouseScrollEventFunction)(World* world, Vector2 offset);
typedef void (*WorldMouseMoveFunction)(World* world, Vector2 position);
typedef void (*WorldCleanFunction)(World* world);

typedef void (*SystemDrawFunction)(System* system);
typedef void (*SystemUpdateFunction)(System* system, float dt);
typedef void (*SystemKeyEventFunction)(System* system, int key, InputAction action);
typedef void (*SystemMouseButtonEventFunction)(System* system, int key, InputAction action);
typedef void (*SystemMouseScrollEventFunction)(System* system, Vector2 offset);
typedef void (*SystemMouseMoveFunction)(System* system, Vector2 position);
typedef void (*SystemCleanFunction)(System* system);

DECLARE_HASHMAP(int, BOOL, KeyMap);
DECLARE_HASHMAP(EntityID, size_t, EntityMap);
DECLARE_HASHMAP(size_t, ComponentStorage*, StorageMap);
DECLARE_ARRLIST(EntityID);
DECLARE_ARRLIST_NAMED(ScenePtr, Scene*);
DECLARE_ARRLIST_NAMED(WorldPtr, World*);
DECLARE_ARRLIST_NAMED(ComponentStoragePtr, ComponentStorage*);
DECLARE_ARRLIST_NAMED(SystemPtr, System*);

#undef DECLARE

#endif
