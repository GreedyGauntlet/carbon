#ifndef SCENE_H
#define SCENE_H

#include "data/declarations.h"

Scene* GenerateScene(const char* name, SceneInitializeFunction init, SceneCleanFunction clean);

void WipeScene(Scene* scene);

void DestroyScene(Scene* scene);

void ResetScene(Scene* scene);

void AddWorld(Scene* scene, World* world);

#endif
