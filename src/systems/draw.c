#include "draw.h"
#include "ui/panels/viewport.h"
#include "data/definitions.h"
#include "systems/system.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "core/world.h"
#include "core/config.h"
#include <easysort.h>
#include <raymath.h>
#include <float.h>

DECLARE_EASYSORT(EntityID);
IMPL_EASYSORT(EntityID);

typedef void (*DrawComponentFunc)(Entity, Vector2);

static World* g_current_world = NULL;

static float ExtractZValue(EntityID e) {
    Entity entity = (Entity){ e, g_current_world };
    return EntityPosition(entity)->z; 
}

static void DrawImageComponent(Entity e, Vector2 origin) {
    ImageComponent* ic = GetComponent(e, ImageComponent);
    TransformComponent* tc = GetComponent(e, TransformComponent);
    DrawTexturePro(
        ic->texture,
        (Rectangle){0, 0, ic->texture.width, ic->texture.height},
        (Rectangle){
            tc->translation.x - tc->scale.x/2.0f + origin.x,
            tc->translation.y - tc->scale.y/2.0f + origin.y,
            tc->scale.x, tc->scale.y},
        (Vector2){0, 0}, 0, WHITE);
}

static void DrawTextComponent(Entity e, Vector2 origin) {
    TextComponent* tc = GetComponent(e, TextComponent);
    Font default_font = GetFontDefault();
    Vector2 text_size = MeasureTextEx(default_font, tc->text, tc->size, 2);
    Vector2 tpos = (Vector2){ EntityPosition(e)->x + origin.x, EntityPosition(e)->y + origin.y };
    tpos.y -= text_size.y / 2.0f;
    switch (tc->alignment) {
        case TEXT_ALIGN_CENTER:
            tpos.x -= text_size.x / 2.0f;
            break;
        case TEXT_ALIGN_LEFT: break;
        case TEXT_ALIGN_RIGHT:
            tpos.x -= text_size.x;
            break;
        default: break;
    }
    DrawTextEx(default_font, tc->text, tpos, tc->size, 2, tc->color);
}

static void DrawShapeComponent(Entity e, Vector2 origin) {
    ShapeComponent* sc = GetComponent(e, ShapeComponent);
    TransformComponent* tc = GetComponent(e, TransformComponent);
    if (sc->type == RECTANGLE_SHAPE) {
        DrawRectangle(tc->translation.x - (tc->scale.x / 2.0f) + origin.x,
                      tc->translation.y - (tc->scale.y / 2.0f) + origin.y,
                      tc->scale.x, tc->scale.y, sc->color);
    } else {
        float radius = (tc->scale.x + tc->scale.y) / 4.0f;
        DrawCircle(tc->translation.x + origin.x, tc->translation.y + origin.y, radius, sc->color);
    }
}

static Vector2 AnchorCoordinate(Entity e) {
    Vector2 slice = GetViewportSlice();
    Vector2 tpos;
    switch (GetComponent(e, AnchorComponent)->anchor) {
        case CENTER_ANCHOR:
            tpos = (Vector2){ slice.x / 2.0f, slice.y / 2.0f };
            break;
        case ML_ANCHOR:
            tpos = (Vector2){ 0, slice.y / 2.0f };
            break;
        case MR_ANCHOR:
            tpos = (Vector2){ slice.x, slice.y / 2.0f };
            break;
        case TL_ANCHOR:
            tpos = (Vector2){ 0, 0 };
            break;
        case TR_ANCHOR:
            tpos = (Vector2){ slice.x, 0 };
            break;
        case BR_ANCHOR:
            tpos = slice;
            break;
        case BL_ANCHOR:
            tpos = (Vector2){ 0, slice.y };
            break;
        case TM_ANCHOR:
            tpos = (Vector2){ slice.x / 2.0f, 0 };
            break;
        case BM_ANCHOR:
            tpos = (Vector2){ slice.x / 2.0f, slice.y };
            break;
        default: break;
    }
    return tpos;
}

static void DrawDrawSystem(System* system) {
    g_current_world = system->context;
    ARRLIST_EntityID* images = GetEntities(system->context, ImageComponent);
    ARRLIST_EntityID* shapes = GetEntities(system->context, ShapeComponent);
    ARRLIST_EntityID* texts = GetEntities(system->context, TextComponent);
    ARRLIST_EntityID anchors = { 0 };
    ARRLIST_int anchorfuncs = { 0 };
    #define DRAWTYPES 3
    ARRLIST_EntityID* lists[DRAWTYPES] = { images, shapes, texts };
    DrawComponentFunc funcs[DRAWTYPES] = { DrawImageComponent, DrawShapeComponent, DrawTextComponent };
    size_t indices[DRAWTYPES] = { 0, 0, 0 };
    Config()->camera.offset = (Vector2){ GetViewportSlice().x / 2.0f, GetViewportSlice().y / 2.0f };
    BeginMode2D(Config()->camera);
    while (TRUE) {
        float zs[DRAWTYPES] = { FLT_MAX, FLT_MAX, FLT_MAX };
        BOOL done = TRUE;
        for (int i = 0; i < DRAWTYPES; i++) {
            if (lists[i] && indices[i] < lists[i]->size) {
                done = FALSE;
                zs[i] = ExtractZValue(lists[i]->data[indices[i]]);
            }
        }
        if (done) break;
        int min = 0;
        for (int i = 1; i < DRAWTYPES; i++) {
            if (zs[i] < zs[min]) {
                min = i;
            }
        }
        Entity e = (Entity){ lists[min]->data[indices[min]], system->context };
        if (HasComponent(e, AnchorComponent)) {
            ARRLIST_EntityID_add(&anchors, e.id);
            ARRLIST_int_add(&anchorfuncs, min);
        } else {
            funcs[min](e, (Vector2){ 0, 0 });
        }
        indices[min]++;
    }
    EndMode2D();
    for (size_t i = 0; i < anchors.size; i++) {
        Entity e = (Entity){ anchors.data[i], system->context };
        funcs[anchorfuncs.data[i]](e, AnchorCoordinate(e));
    }
    ARRLIST_EntityID_clear(&anchors);
    ARRLIST_int_clear(&anchorfuncs);
    #undef DRAWTYPES
}

static void UpdateDrawSystem(System* system, float dt) {
    g_current_world = system->context;
    ARRLIST_EntityID* eids = GetEntities(system->context, ImageComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
    eids = GetEntities(system->context, ShapeComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
    eids = GetEntities(system->context, TextComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
}

System* GenerateDrawSystem() {
    return GenerateSystem(DrawDrawSystem, UpdateDrawSystem, NULL, NULL, NULL, NULL, NULL);
}
