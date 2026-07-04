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
    ARRLIST_StaticString_clear(&(scene->assets.texnames));
    ARRLIST_StaticString_clear(&(scene->assets.animnames));
    ARRLIST_StaticString_clear(&(scene->assets.soundnames));
    ARRLIST_StaticString_clear(&(scene->assets.musicnames));
    for (size_t i = 0; i < scene->assets.textures.size; i++) UnloadTexture(scene->assets.textures.data[i]);
    for (size_t i = 0; i < scene->assets.sounds.size; i++) {
        StopSound(scene->assets.sounds.data[i]);
        UnloadSound(scene->assets.sounds.data[i]);
    }
    ARRLIST_Texture2D_clear(&(scene->assets.textures));
    ARRLIST_Sound_clear(&(scene->assets.sounds));
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

size_t PackTexture(Scene* scene, Texture2D texture, const char* name) {
    EZ_ASSERT(FindTexture(scene, name) == (size_t)-1, "A texture with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.texnames), name);
    ARRLIST_Texture2D_add(&(scene->assets.textures), texture);
    return scene->assets.textures.size - 1;
}

size_t FindTexture(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.texnames.size; i++)
        if (strcmp(scene->assets.texnames.data[i], name) == 0) return i;
    return (size_t)-1;
}

size_t PackSound(Scene* scene, Sound sound, const char* name) {
    EZ_ASSERT(FindSound(scene, name) == (size_t)-1, "A sound with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.soundnames), name);
    ARRLIST_Sound_add(&(scene->assets.sounds), sound);
    return scene->assets.sounds.size - 1;
}

size_t FindSound(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.soundnames.size; i++)
        if (strcmp(scene->assets.soundnames.data[i], name) == 0) return i;
    return (size_t)-1;
}
