#ifndef PICK_H
#define PICK_H

#include "data/declarations.h"

void ResizePickTarget();

void BeginPickPass();

void EndPickPass();

BOOL IsPicking();

size_t RegisterPickable(Entity e);

Color PickColorFor(size_t index);

Entity ResolvePick(Vector2 localPos);

void CleanPicking();

#endif
