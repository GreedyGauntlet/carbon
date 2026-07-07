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
    ARRLIST_StaticString_clear(&(scene->assets.animnames));
    ARRLIST_StaticString_clear(&(scene->assets.texpaths));
    ARRLIST_StaticString_clear(&(scene->assets.soundpaths));
    ARRLIST_StaticString_clear(&(scene->assets.musicpaths));
    for (size_t i = 0; i < scene->assets.textures.size; i++) UnloadTexture(scene->assets.textures.data[i]);
    for (size_t i = 0; i < scene->assets.sounds.size; i++) {
        StopSound(scene->assets.sounds.data[i]);
        UnloadSound(scene->assets.sounds.data[i]);
    }
    for (size_t i = 0; i < scene->assets.musics.size; i++) {
        StopMusicStream(scene->assets.musics.data[i]);
        UnloadMusicStream(scene->assets.musics.data[i]);
    }
    ARRLIST_Texture2D_clear(&(scene->assets.textures));
    ARRLIST_Sound_clear(&(scene->assets.sounds));
    ARRLIST_Music_clear(&(scene->assets.musics));
    ARRLIST_Animation_clear(&(scene->assets.animations));
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

size_t PackTexture(Scene* scene, const char* path, const char* name) {
    EZ_ASSERT(FindTexture(scene, name) == (size_t)-1, "A texture with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.texnames), name);
    ARRLIST_StaticString_add(&(scene->assets.texpaths), path);
    ARRLIST_Texture2D_add(&(scene->assets.textures), LoadTexture(path));
    return scene->assets.textures.size - 1;
}

size_t FindTexture(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.texnames.size; i++)
        if (strcmp(scene->assets.texnames.data[i], name) == 0) return i;
    return (size_t)-1;
}

size_t PackSound(Scene* scene, const char* path, const char* name) {
    EZ_ASSERT(FindSound(scene, name) == (size_t)-1, "A sound with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.soundnames), name);
    ARRLIST_StaticString_add(&(scene->assets.soundpaths), path);
    ARRLIST_Sound_add(&(scene->assets.sounds), LoadSound(path));
    return scene->assets.sounds.size - 1;
}

size_t FindSound(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.soundnames.size; i++)
        if (strcmp(scene->assets.soundnames.data[i], name) == 0) return i;
    return (size_t)-1;
}

size_t PackMusic(Scene* scene, const char* path, const char* name) {
    EZ_ASSERT(FindMusic(scene, name) == (size_t)-1, "A stream with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.musicnames), name);
    ARRLIST_StaticString_add(&(scene->assets.musicpaths), path);
    ARRLIST_Music_add(&(scene->assets.musics), LoadMusicStream(path));
    return scene->assets.musics.size - 1;
}

size_t FindMusic(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.musicnames.size; i++)
        if (strcmp(scene->assets.musicnames.data[i], name) == 0) return i;
    return (size_t)-1;
}

size_t PackAnimation(Scene* scene, Animation animation, const char* name) {
    EZ_ASSERT(FindAnimation(scene, name) == (size_t)-1, "An animation with this name has already been packed into this scene");
    ARRLIST_StaticString_add(&(scene->assets.animnames), name);
    ARRLIST_Animation_add(&(scene->assets.animations), animation);
    return scene->assets.animations.size - 1;
}

size_t FindAnimation(Scene* scene, const char* name) {
    for (size_t i = 0; i < scene->assets.animnames.size; i++)
        if (strcmp(scene->assets.animnames.data[i], name) == 0) return i;
    return (size_t)-1;
}
