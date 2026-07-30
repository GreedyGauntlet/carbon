#ifndef EDIT_H
#define EDIT_H

#include "data/declarations.h"

Panel GenerateEditPanel();

void SelectEntity(Entity e);

Entity SelectedEntity();

void SetHoveredEntity(Entity e);

Entity HoveredEntity();

#endif
