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
    ARRLIST_WorldPtr_add(&scene->worlds, world);
}
