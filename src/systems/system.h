#ifndef SYSTEM_H
#define SYSTEM_H

#include "data/declarations.h"

System* GenerateSystem(
        SystemDrawFunction draw,
        SystemUpdateFunction update,
        SystemKeyEventFunction key,
        SystemMouseButtonEventFunction mousebutton,
        SystemMouseScrollEventFunction mousescroll,
        SystemMouseMoveFunction mousemove,
        SystemCleanFunction clean);

void DestroySystem(System* system);

#endif
