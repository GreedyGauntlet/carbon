#ifndef SCENE_H
#define SCENE_H

#include "data/declarations.h"

Scene* GenerateScene(const char* name, SceneInitializeFunction init, SceneCleanFunction clean);

void WipeScene(Scene* scene);

void DestroyScene(Scene* scene);

void ResetScene(Scene* scene);

void AddWorld(Scene* scene, World* world);

size_t PackScript(Scene* scene, Script script, const char* name, const char* description);

size_t FindScript(Scene* scene, const char* name);

size_t PackTexture(Scene* scene, Texture2D texture, const char* name);

size_t FindTexture(Scene* scene, const char* name);

size_t PackSound(Scene* scene, Sound sound, const char* name);

size_t FindSound(Scene* scene, const char* name);

size_t PackMusic(Scene* scene, Music music, const char* name);

size_t FindMusic(Scene* scene, const char* name);

#endif
