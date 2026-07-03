#include "scene.h"
#include "data/definitions.h"
#include "core/world.h"

Scene* GenerateScene(const char* name, SceneInitializeFunction init, SceneCleanFunction clean) {
    Scene* scene = EZ_ALLOC(1, sizeof(Scene));
    scene->name = name;
    scene->init = init;
    scene->clean = clean;
    if (scene->init) scene->init(scene);
    return scene;
}

void WipeScene(Scene* scene) {
    for (size_t i = 0; i < scene->worlds.size; i++) DestroyWorld(scene->worlds.data[i]);
    ARRLIST_WorldPtr_clear(&scene->worlds);
    ARRLIST_StaticString_clear(&(scene->scripts.names));
    ARRLIST_StaticString_clear(&(scene->scripts.descriptions));
    ARRLIST_Script_clear(&(scene->scripts.scripts));
}

void DestroyScene(Scene* scene) {
    if (scene->clean) scene->clean(scene);
    WipeScene(scene);
    EZ_FREE(scene);
}

void ResetScene(Scene* scene) {
    if (scene->clean) scene->clean(scene);
    if (scene->init) scene->init(scene);
}

void AddWorld(Scene* scene, World* world) {
    EZ_ASSERT(world->parent == NULL, "World already is linked to an existing scene");
    world->parent = scene;
    ARRLIST_WorldPtr_add(&scene->worlds, world);
}

size_t PackScript(Scene* scene, Script script, const char* name, const char* description) {
    EZ_ASSERT(FindScript(scene, name) == (size_t)-1, "A script with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->scripts.names), name);
    ARRLIST_StaticString_add(&(scene->scripts.descriptions), description);
    ARRLIST_Script_add(&(scene->scripts.scripts), script);
    return scene->scripts.scripts.size - 1;
}

size_t FindScript(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->scripts.names.size; i++)
        if (strcmp(scene->scripts.names.data[i], name) == 0) return i;
    return (size_t)-1;
}
