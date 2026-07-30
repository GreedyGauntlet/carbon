#include "draw.h"
#include "ui/panels/viewport.h"
#include "ui/panels/edit.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "systems/system.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "core/application.h"
#include "core/world.h"
#include "core/config.h"
#include "core/scene.h"
#include "util/logger.h"
#include "util/pick.h"
#include <easysort.h>
#include <raymath.h>
#include <float.h>

DECLARE_EASYSORT(EntityID);
IMPL_EASYSORT(EntityID);

typedef void (*DrawComponentFunc)(Entity, Vector2);

static World* g_current_world = NULL;
static BOOL g_viewcam_locked = FALSE;

static void DrawPickGeometry(Entity e, Vector2 origin) {
    Color id_color = PickColorFor(RegisterPickable(e));
    Vector3 translation = GetWorldPosition(e);
    Vector2 scale = GetWorldScale(e);
    if (HasComponent(e, ShapeComponent) && GetComponent(e, ShapeComponent)->type == CIRCLE_SHAPE) {
        float radius = (scale.x + scale.y) / 4.0f;
        DrawCircle(translation.x + origin.x, translation.y + origin.y, radius, id_color);
    }
    if (HasComponent(e, TextComponent)) {
        TextComponent* tc = GetComponent(e, TextComponent);
        Vector2 size = MeasureTextEx(GetFontDefault(), tc->text, tc->size * scale.x, 2);
        Vector2 tpos = { translation.x + origin.x, translation.y + origin.y - size.y / 2.0f };
        switch (tc->alignment) {
            case TEXT_ALIGN_CENTER: tpos.x -= size.x / 2.0f; break;
            case TEXT_ALIGN_RIGHT:  tpos.x -= size.x; break;
            default: break;
        }
        DrawRectangle(tpos.x, tpos.y, size.x, size.y, id_color);
    }
    float rotation =
        (HasComponent(e, TextureComponent) ||
         HasComponent(e, AnimationComponent) ||
        (HasComponent(e, ShapeComponent) && GetComponent(e, ShapeComponent)->type == RECTANGLE_SHAPE))
            ? GetWorldRotation(e) : 0.0f;
    DrawRectanglePro(
        (Rectangle){ translation.x - scale.x/2.0f + origin.x, translation.y - scale.y/2.0f + origin.y, scale.x, scale.y },
        (Vector2){ 0, 0 }, rotation, id_color);
}

static void DrawEntityHighlight(Entity e, Vector2 origin) {
    Entity sel = SelectedEntity(), hov = HoveredEntity();
    BOOL is_selected = sel.id == e.id && sel.context == e.context;
    BOOL is_hovered = !is_selected && hov.id == e.id && hov.context == e.context;
    if (!is_selected && !is_hovered) return;
    Color color = is_selected ? MappedColor(ENTITY_SELECTED_OUTLINE) : MappedColor(ENTITY_HOVERED_OUTLINE);

    Vector3 translation = GetWorldPosition(e);
    Vector2 scale = GetWorldScale(e);
    if (HasComponent(e, ShapeComponent) && GetComponent(e, ShapeComponent)->type == CIRCLE_SHAPE) {
        float radius = (scale.x + scale.y) / 4.0f;
        DrawRing((Vector2){ translation.x + origin.x, translation.y + origin.y }, radius, radius + 2.0f, 0, 360, 36, color);
    }
    float rotation = (HasComponent(e, TextureComponent) || HasComponent(e, AnimationComponent))
        ? GetWorldRotation(e) : 0.0f;
    Vector2 pivot = { translation.x - scale.x/2.0f + origin.x, translation.y - scale.y/2.0f + origin.y };
    float rad = rotation * DEG2RAD, c = cosf(rad), s = sinf(rad);
    Vector2 local[4] = { {0, 0}, {scale.x, 0}, {scale.x, scale.y}, {0, scale.y} };
    Vector2 corners[4];
    if (HasComponent(e, TextComponent)) {
        TextComponent* tc = GetComponent(e, TextComponent);
        Vector2 size = MeasureTextEx(GetFontDefault(), tc->text, tc->size * scale.x, 2);
        Vector2 tpos = { translation.x + origin.x, translation.y + origin.y - size.y / 2.0f };
        switch (tc->alignment) {
            case TEXT_ALIGN_CENTER: tpos.x -= size.x / 2.0f; break;
            case TEXT_ALIGN_RIGHT:  tpos.x -= size.x; break;
            default: break;
        }
        pivot = tpos;
        local[1] = (Vector2){ size.x, 0 };
        local[2] = (Vector2){ size.x, size.y };
        local[3] = (Vector2){ 0, size.y };
    }
    for (int i = 0; i < 4; i++)
        corners[i] = (Vector2){ pivot.x + local[i].x*c - local[i].y*s, pivot.y + local[i].x*s + local[i].y*c };
    for (int i = 0; i < 4; i++)
        DrawLineEx(corners[i], corners[(i+1)%4], 2.0f, color);
}

static float ExtractZValue(EntityID e) {
    Entity entity = (Entity){ e, g_current_world };
    return GetWorldPosition(entity).z; 
}

static void DrawAnimationComponent(Entity e, Vector2 origin) {
    AnimationComponent* ac = GetComponent(e, AnimationComponent);
    Vector3 translation = GetWorldPosition(e);
    Vector2 scale = GetWorldScale(e);
    if (ac->id != (size_t)-1) {
        EZ_ASSERT(ac->id < e.context->parent->assets.animations.size, "Invalid Animation ID [%d] detected", (int)ac->id);
        Animation anim = e.context->parent->assets.animations.data[ac->id];
        if (anim.frames == 0) {
            logwarn("Cannot animate an animation with 0 frames");
            return;
        }
        if (anim.fps == 0) {
            logwarn("Cannot animate an animation with 0 fps");
            return;
        }
        Texture2D sheet;
        if (anim.sheet != (size_t)-1) {
            EZ_ASSERT(anim.sheet < e.context->parent->assets.textures.size, "Invalid Texture ID [%d] detected", (int)anim.sheet);
            sheet = e.context->parent->assets.textures.data[anim.sheet];
        } else if (HasComponent(e, TextureComponent)) {
            TextureComponent* tc = GetComponent(e, TextureComponent);
            if (tc->id != (size_t)-1) {
                EZ_ASSERT(tc->id < e.context->parent->assets.textures.size, "Invalid Texture ID [%d] detected", (int)tc->id);
                sheet = e.context->parent->assets.textures.data[tc->id];
            } else {
                logwarn("Entity with AnimationComponent does not have an animation sheet");
                return;
            }
        } else {
            logwarn("Entity with AnimationComponent does not have an animation sheet");
            return;
        }
        float frametime = 1.0f / anim.fps;
        frametime = ac->time / frametime;
        size_t frame = (size_t)frametime;
        frame = !ac->loop && frame >= anim.frames ? anim.frames - 1 : frame % anim.frames;
        DrawTexturePro(
            sheet,
            (Rectangle){
                anim.origin.x + (ac->flipped ? anim.width : 0) + (frame * anim.width),
                anim.origin.y + (ac->flopped ? anim.height : 0),
                ((float)anim.width) * (ac->flipped ? -1.0f : 1.0f),
                ((float)anim.height) * (ac->flopped ? -1.0f : 1.0f)
            },
            (Rectangle){
                translation.x - (scale.x / 2.0f) + origin.x,
                translation.y - (scale.y / 2.0f) + origin.y,
                scale.x, scale.y
            },
            (Vector2){ 0, 0 }, GetWorldRotation(e), WHITE
        );
        if (!ac->paused) ac->time += ac->speed * GetFrameTime() * (IsFast() ? Config()->ffspeed : 1.0f);
    }
}

static void DrawTextureComponent(Entity e, Vector2 origin) {
    TextureComponent* tc = GetComponent(e, TextureComponent);
    if (tc->id != (size_t)-1) {
        EZ_ASSERT(tc->id < e.context->parent->assets.textures.size, "Invalid Texture ID [%d] detected", (int)tc->id);
        Texture2D tex = e.context->parent->assets.textures.data[tc->id];
        Vector3 translation = GetWorldPosition(e);
        Vector2 scale = GetWorldScale(e);
        DrawTexturePro(
            tex,
            (Rectangle){0, 0, tex.width, tex.height},
            (Rectangle){
                translation.x - scale.x/2.0f + origin.x,
                translation.y - scale.y/2.0f + origin.y,
                scale.x, scale.y},
            (Vector2){0, 0}, GetWorldRotation(e), WHITE);
    }
}

static void DrawTextComponent(Entity e, Vector2 origin) {
    TextComponent* tc = GetComponent(e, TextComponent);
    Font default_font = GetFontDefault();
    Vector2 text_size = MeasureTextEx(default_font, tc->text, tc->size * GetWorldScale(e).x, 2);
    Vector3 translation = GetWorldPosition(e);
    Vector2 tpos = (Vector2){ translation.x + origin.x, translation.y + origin.y };
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
    DrawTextEx(default_font, tc->text, tpos, tc->size * GetWorldScale(e).x, 2, tc->color);
}

static void DrawShapeComponent(Entity e, Vector2 origin) {
    ShapeComponent* sc = GetComponent(e, ShapeComponent);
    Vector3 translation = GetWorldPosition(e);
    Vector2 scale = GetWorldScale(e);
    if (sc->type == RECTANGLE_SHAPE) {
        DrawRectangle(translation.x - (scale.x / 2.0f) + origin.x,
                      translation.y - (scale.y / 2.0f) + origin.y,
                      scale.x, scale.y, sc->color);
    } else {
        float radius = (scale.x + scale.y) / 4.0f;
        DrawCircle(translation.x + origin.x, translation.y + origin.y, radius, sc->color);
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
    ARRLIST_EntityID* eids = GetEntities(system->context, TextureComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
    eids = GetEntities(system->context, ShapeComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
    eids = GetEntities(system->context, TextComponent);
    if (eids) EasySort_EntityID(eids, ExtractZValue);
    ARRLIST_EntityID* images = GetEntities(system->context, TextureComponent);
    ARRLIST_EntityID* shapes = GetEntities(system->context, ShapeComponent);
    ARRLIST_EntityID* texts = GetEntities(system->context, TextComponent);
    ARRLIST_EntityID* animations = GetEntities(system->context, AnimationComponent);
    ARRLIST_EntityID* cameras = GetEntities(system->context, CameraComponent);
    ARRLIST_EntityID anchors = { 0 };
    ARRLIST_int anchorfuncs = { 0 };
    #define DRAWTYPES 4
    ARRLIST_EntityID* lists[DRAWTYPES] = { images, shapes, texts, animations };
    DrawComponentFunc funcs[DRAWTYPES] = { DrawTextureComponent, DrawShapeComponent, DrawTextComponent, DrawAnimationComponent };
    size_t indices[DRAWTYPES] = { 0 };
    Config()->camera.offset = (Vector2){ GetViewportSlice().x / 2.0f, GetViewportSlice().y / 2.0f };
    Camera2D camera = Config()->camera;
    g_viewcam_locked = FALSE;
    if (Playing() && cameras) {
        for (size_t i = 0; i < cameras->size; i++) {
            Entity e = (Entity){ cameras->data[i], system->context };
            CameraComponent* cc = GetComponent(e, CameraComponent);
            if (cc->enabled) {
                camera.target = Vector2Add((Vector2){ GetWorldPosition(e).x, GetWorldPosition(e).y }, cc->offset);
                camera.rotation = cc->rotation + GetWorldRotation(e);
                camera.zoom = cc->zoom;
                g_viewcam_locked = TRUE;
                break;
            }
        }
    }
    BeginMode2D(camera);
    while (TRUE) {
        float zs[DRAWTYPES] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
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
            if (IsPicking()) {
                DrawPickGeometry(e, (Vector2){ 0, 0 });
            } else {
                BOOL useshader = HasComponent(e, ShaderComponent);
                if (useshader) BeginShaderMode(GetShader(system->context->parent, GetComponent(e, ShaderComponent)->id));
                funcs[min](e, (Vector2){ 0, 0 });
                if (useshader) EndShaderMode();
                if (Config()->enableclickselection && !Playing()) DrawEntityHighlight(e, (Vector2){ 0, 0 });
            }
        }
        indices[min]++;
    }
    EndMode2D();
    for (size_t i = 0; i < anchors.size; i++) {
        Entity e = (Entity){ anchors.data[i], system->context };
        if (IsPicking()) {
            DrawPickGeometry(e, AnchorCoordinate(e));
        } else {
            BOOL useshader = HasComponent(e, ShaderComponent);
            if (useshader) BeginShaderMode(GetShader(system->context->parent, GetComponent(e, ShaderComponent)->id));
            funcs[anchorfuncs.data[i]](e, AnchorCoordinate(e));
            if (useshader) EndShaderMode();
            if (Config()->enableclickselection && !Playing()) DrawEntityHighlight(e, AnchorCoordinate(e));
        }
    }
    ARRLIST_EntityID_clear(&anchors);
    ARRLIST_int_clear(&anchorfuncs);
    #undef DRAWTYPES
}

System* GenerateDrawSystem() {
    return GenerateSystem(DrawDrawSystem, NULL, NULL, NULL, NULL, NULL, NULL);
}

BOOL ViewCameraLocked() {
    return g_viewcam_locked;
}
