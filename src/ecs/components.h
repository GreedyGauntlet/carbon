#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "data/declarations.h"

#define EXPOSE_COMPONENT(name) enum { name##_TYPE = __COUNTER__ - CLEAN_COMPONENT_COUNTER - 1 }; struct name; typedef struct name name; struct name
#define START_COMPONENT_DEFINITIONS() enum { CLEAN_COMPONENT_COUNTER = __COUNTER__ } 
START_COMPONENT_DEFINITIONS();

typedef void (*ScriptComponentInitializeFunction)(const Entity e);
typedef void (*ScriptComponentDrawFunction)(const Entity e);
typedef void (*ScriptComponentUpdateFunction)(const Entity e, float dt);
typedef void (*ScriptComponentCleanFunction)(const Entity e);
typedef void (*ScriptComponentKeyEventFunction)(const Entity e, int key, InputAction action);
typedef void (*ScriptComponentMouseButtonEventFunction)(const Entity e, int key, InputAction action);
typedef void (*ScriptComponentMouseScrollEventFunction)(const Entity e, Vector2 offset);
typedef void (*ScriptComponentMouseMoveFunction)(const Entity e, Vector2 position);

typedef enum {
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_RIGHT
} TextAlignment;

typedef enum {
    CENTER_ANCHOR,
    ML_ANCHOR,
    MR_ANCHOR,
    TL_ANCHOR,
    TR_ANCHOR,
    BR_ANCHOR,
    BL_ANCHOR,
    TM_ANCHOR,
    BM_ANCHOR
} ViewportAnchor;

typedef enum {
    RECTANGLE_SHAPE,
    CIRCLE_SHAPE
} ShapeType;

EXPOSE_COMPONENT(TagComponent) {
    const char* tag;
};

EXPOSE_COMPONENT(ImageComponent) {
    Texture2D texture;
};

EXPOSE_COMPONENT(TransformComponent) {
    Vector3 translation;
    float rotation;
    Vector2 scale;
};

EXPOSE_COMPONENT(TextComponent) {
    char* text;
    TextAlignment alignment;
    ViewportAnchor anchor;
    Color color;
    float size;
};

EXPOSE_COMPONENT(CameraComponent) {
    BOOL enabled;
    Vector2 offset;
    float rotation;
};

EXPOSE_COMPONENT(ShapeComponent) {
    ShapeType type;
    Color color;
};

EXPOSE_COMPONENT(ScriptComponent) {
    ScriptComponentInitializeFunction init;
    ScriptComponentUpdateFunction update;
    ScriptComponentDrawFunction draw;
    ScriptComponentCleanFunction clean;
    ScriptComponentKeyEventFunction key;
    ScriptComponentMouseButtonEventFunction mousebutton;
    ScriptComponentMouseMoveFunction mousemove;
    ScriptComponentMouseScrollEventFunction mousescroll;
    void* arbitrary;
    BOOL initialized;
};

#endif
