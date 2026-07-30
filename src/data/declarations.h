#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <raylib.h>
#include <stdint.h>
#include <easyobjects.h>
#include <easybasics.h>

#define DECLARE(name) struct name; typedef struct name name;

#define INVALID_ENTITY 0
#define MAX_NAME_LEN 256
#define MAX_LINE_WIDTH 2048

typedef uint64_t EntityID;

typedef enum {
    INPUTPRESS,
    INPUTRELEASE,
    INPUTDOWN,
} InputAction;

typedef enum {
    LEVEL_NONE  = 0, // no logs at all
    LEVEL_TRACE = 1, // verbose information
    LEVEL_INFO  = 2, // specifically requested information
    LEVEL_WARN  = 3, // information hinting towards failing behavior
    LEVEL_ERROR = 4  // information explicitly stating failure
} MessageLevel;

DECLARE(Application);
DECLARE(Scene);
DECLARE(World);
DECLARE(Entity);
DECLARE(System);
DECLARE(ComponentStorage);
DECLARE(Registry);
DECLARE(Panel);
DECLARE(UI);
DECLARE(Popup);
DECLARE(Notification);
DECLARE(AssetPack);
DECLARE(ScriptPack);
DECLARE(Script);
DECLARE(Animation);
DECLARE(UIConfig);

typedef void (*SceneInitializeFunction)(Scene* scene);
typedef void (*SceneCleanFunction)(Scene* scene);

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

typedef void (*ScriptComponentInitializeFunction)(const Entity e);
typedef void (*ScriptComponentDrawFunction)(const Entity e);
typedef void (*ScriptComponentUpdateFunction)(const Entity e, float dt);
typedef void (*ScriptComponentCleanFunction)(const Entity e);
typedef void (*ScriptComponentKeyEventFunction)(const Entity e, int key, InputAction action);
typedef void (*ScriptComponentMouseButtonEventFunction)(const Entity e, int key, InputAction action);
typedef void (*ScriptComponentMouseScrollEventFunction)(const Entity e, Vector2 offset);
typedef void (*ScriptComponentMouseMoveFunction)(const Entity e, Vector2 position);

typedef void (*PanelFunction)(float width, float height);
typedef void (*CleanFunction)(void);
typedef void (*SelectFunction)(size_t index);
typedef size_t (*DropdownSelectFunction)(void* data, size_t index, BOOL cancel);

typedef int (*PopupFunction)(size_t, size_t, size_t, size_t);

DECLARE_ARRLIST(EntityID);
DECLARE_ARRLIST(Panel);
DECLARE_ARRLIST(Notification);
DECLARE_ARRLIST(Script);
DECLARE_ARRLIST(Texture2D);
DECLARE_ARRLIST(Sound);
DECLARE_ARRLIST(Music);
DECLARE_ARRLIST(Animation);
DECLARE_ARRLIST(UIConfig);
DECLARE_ARRLIST(Shader);
DECLARE_ARRLIST_NAMED(ScenePtr, Scene*);
DECLARE_ARRLIST_NAMED(WorldPtr, World*);
DECLARE_ARRLIST_NAMED(ComponentStoragePtr, ComponentStorage*);
DECLARE_ARRLIST_NAMED(SystemPtr, System*);
DECLARE_ARRLIST_NAMED(StaticString, const char*);
DECLARE_ARRLIST_NAMED(DynamicString, char*);
DECLARE_HASHMAP(int, BOOL, KeyMap);
DECLARE_HASHMAP(EntityID, size_t, EntityMap);
DECLARE_HASHMAP(size_t, ComponentStorage*, StorageMap);
DECLARE_HASHMAP(EntityID, EntityID, Parents);
DECLARE_HASHMAP(EntityID, ARRLIST_EntityID*, Children);

#undef DECLARE

struct Script {
    ScriptComponentInitializeFunction init;
    ScriptComponentUpdateFunction update;
    ScriptComponentDrawFunction draw;
    ScriptComponentCleanFunction clean;
    ScriptComponentKeyEventFunction key;
    ScriptComponentMouseButtonEventFunction mousebutton;
    ScriptComponentMouseMoveFunction mousemove;
    ScriptComponentMouseScrollEventFunction mousescroll;
    void* arbitrary;
    BOOL initialized;
};

struct Animation {
    size_t sheet; // if -1, fall back to texture component
    size_t frames;
    size_t width;
    size_t height;
    Vector2 origin;
    float fps;
};

#endif
