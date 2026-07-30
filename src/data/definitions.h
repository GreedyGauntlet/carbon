#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "data/declarations.h"

#define MAX_NOTIFICATION_SIZE 2048

struct Application {
    const char* name;
    const char* goodbye;
    ARRLIST_ScenePtr scenes;
    size_t current;
    HASHMAP_KeyMap keymap;
    ARRLIST_int keylist;
    UI* ui;

    #ifndef PROD_BUILD
    size_t memory;
    #endif
};

struct ScriptPack {
    ARRLIST_StaticString names;
    ARRLIST_StaticString descriptions;
    ARRLIST_Script scripts;
};

struct AssetPack {
    ARRLIST_StaticString texnames;
    ARRLIST_StaticString texpaths;
    ARRLIST_Texture2D textures;
    ARRLIST_StaticString animnames;
    ARRLIST_Animation animations;
    ARRLIST_StaticString soundnames;
    ARRLIST_StaticString soundpaths;
    ARRLIST_Sound sounds;
    ARRLIST_StaticString musicnames;
    ARRLIST_StaticString musicpaths;
    ARRLIST_Music musics;
    ARRLIST_Shader shaders;
    ARRLIST_StaticString shadernames;
    ARRLIST_StaticString vertexshaderpaths;
    ARRLIST_StaticString fragmentshaderpaths;
};

struct Scene {
    const char* name;
    ARRLIST_WorldPtr worlds;
    SceneInitializeFunction init;
    SceneCleanFunction clean;
    ScriptPack scripts;
    AssetPack assets;
};

struct World {
    const char* name;
    WorldDrawFunction draw;
    WorldUpdateFunction update;
    WorldKeyEventFunction key;
    WorldMouseButtonEventFunction mousebutton;
    WorldMouseScrollEventFunction mousescroll;
    WorldMouseMoveFunction mousemove;
    WorldCleanFunction clean;
    Registry* registry;
    ARRLIST_SystemPtr systems;
    ARRLIST_EntityID removal;
    HASHMAP_Parents parents;
    HASHMAP_Children children;
    Scene* parent;
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

struct Panel {
    char name[MAX_NAME_LEN];
    RenderTexture2D texture;
    PanelFunction draw;
    PanelFunction update;
    CleanFunction clean;
	BOOL flush;
};

struct UI {
    void* left;
    void* right;
    size_t divide;
    size_t x;
    size_t y;
    size_t w;
    size_t h;
	ARRLIST_Panel panels;
	size_t selected;
    BOOL vertical;
};

struct Popup {
    PopupFunction behavior;
    CleanFunction clean;
    size_t options;
    void* results;
};

struct Notification {
    char message[MAX_NOTIFICATION_SIZE];
    MessageLevel level;
};

struct UIConfig {
    char name[MAX_NAME_LEN];
    float divide;
    BOOL vertical;
    BOOL left;
    BOOL right;
	BOOL vine;
};

#endif
