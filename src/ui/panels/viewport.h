#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "data/declarations.h"

Panel GenerateViewportPanel();

Vector2 GetViewportSlice();

Vector2 GetViewportPosition();

void SetViewportSlice(Vector2 slice);

void SetViewportPosition(Vector2 pos);

#endif
