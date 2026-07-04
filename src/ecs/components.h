#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "data/declarations.h"

#define EXPOSE_COMPONENT(name) enum { name##_TYPE = __COUNTER__ - CLEAN_COMPONENT_COUNTER - 1 }; struct name; typedef struct name name; struct name
#define START_COMPONENT_DEFINITIONS() enum { CLEAN_COMPONENT_COUNTER = __COUNTER__ } 
START_COMPONENT_DEFINITIONS();

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
    BOOL metacollapsed;
};

EXPOSE_COMPONENT(TextureComponent) {
    size_t id;
};

EXPOSE_COMPONENT(TransformComponent) {
    Vector3 translation;
    float rotation;
    Vector2 scale;
};

EXPOSE_COMPONENT(TextComponent) {
    char* text;
    TextAlignment alignment;
    Color color;
    float size;
};

EXPOSE_COMPONENT(CameraComponent) {
    BOOL enabled;
    Vector2 offset;
    float rotation;
    float zoom;
};

EXPOSE_COMPONENT(ShapeComponent) {
    ShapeType type;
    Color color;
};

EXPOSE_COMPONENT(ScriptComponent) {
    size_t id;
};

EXPOSE_COMPONENT(AnchorComponent) {
    ViewportAnchor anchor;
};

EXPOSE_COMPONENT(ParentComponent) {
    EntityID parentid;
};

#endif
