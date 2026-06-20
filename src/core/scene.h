#ifndef SCENE_H
#define SCENE_H

#include "data/declarations.h"

Scene* GenerateScene(const char* name);

void DestroyScene(Scene* scene);

void AddWorld(Scene* scene, World* world);

#endif
