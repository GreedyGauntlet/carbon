#include "edit.h"
#include "data/definitions.h"
#include "ui/ui.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "core/application.h"
#include "core/world.h"
#include <ui/extra.h>
#include <util/logger.h>
#include <data/colors.h>
#include <data/input.h>
#include <float.h>
#include <raymath.h>

#define LEFT_COLUMN_WIDTH 150

static Entity g_selected = { 0 };
static Entity g_hovered = { 0 };
static const char* g_audio_command_labels[] = { "Send Command", "Play", "Pause", "Resume", "Stop" };
static const char* g_shape_labels[] = { "Rectangle", "Circle" };
static float g_scrolldiff = 0.0f;

static void DrawTagComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TagComponent)) return;
    TagComponent* tc = GetComponent(g_selected, TagComponent);
    UIMoveCursor(0, 5);
    DrawRectangleGradientH(UIGetCursor().x - 3, UIGetCursor().y - 3, width - 14, 26, (Color){ 255, 255, 255, 190 }, (Color){ 0 });
    UIDrawText(tc->tag);
}

static BOOL DrawTransformComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TransformComponent)) return FALSE;
    UIDividerLabeled(width, "Transform");
    TransformComponent* tc = GetComponent(g_selected, TransformComponent);
    BOOL edited = FALSE;
    Color colors[] = {GOLD, MAGENTA, SKYBLUE};
    edited |= UITriplet("xyz", UI_FLOATS, &(tc->translation.x), &(tc->translation.y), &(tc->translation.z), (UIMultiValue){ ._float = -FLT_MAX }, (UIMultiValue){ ._float = FLT_MAX }, (UIMultiValue){ ._float = 0.1f }, (UIMultiValue){ ._float = 0.0f }, width);
    edited |= UIColoredTriplet(colors, "whr", UI_FLOATS, &(tc->scale.x), &(tc->scale.y), &(tc->rotation), (UIMultiValue){ ._float = -FLT_MAX }, (UIMultiValue){ ._float = FLT_MAX }, (UIMultiValue){ ._float = 0.1f }, (UIMultiValue){ ._float = 0.0f }, width);
    return edited;
}

static BOOL DrawAnchorComponentUI(float width, float height) {
    if (!HasComponent(g_selected, AnchorComponent)) return FALSE;
    UIDividerLabeled(width, "Anchor");
    AnchorComponent* ac = GetComponent(g_selected, AnchorComponent);
    UIMoveCursor(0, 25);
    UIDrawText("Anchor Alignment");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y - 25, 2, 65, MappedColor(UI_SUBTLE_TEXT_COLOR));
    UIMoveCursor(0, -25);
    BOOL edited = FALSE;
    float boxw = 20.0f;
    float gapw = 2.0f;
    ViewportAnchor anchors[] = { TL_ANCHOR, ML_ANCHOR, BL_ANCHOR, TM_ANCHOR, CENTER_ANCHOR, BM_ANCHOR, TR_ANCHOR, MR_ANCHOR, BR_ANCHOR };
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            DrawRectangle(UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y + (j * (boxw + gapw)), boxw, boxw, (Color){ 255, 255, 255, 150 });
            if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y + (j * (boxw + gapw)), boxw, boxw})) {
                DrawRectangle(UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y + (j * (boxw + gapw)), boxw, boxw, (Color){ 255, 255, 255, 170 });
                if (InputButtonPressed(IK_MOUSELEFT)) {
                    ac->anchor = anchors[i * 3 + j];
                    edited = TRUE;
                }
            }
            if (ac->anchor == anchors[i * 3 + j])
                DrawCircle(UIGetCursor().x + (i * (boxw + gapw)) + (boxw/2.0f), UIGetCursor().y + (j * (boxw + gapw)) + (boxw/2.0f), 5, RED);
        }
    }
    UIMoveCursor(-UIGetCursor().x + 10, 65);
    return edited;
}

static size_t DropdownSelectTexture(void* data, size_t index, BOOL cancel) {
    TextureComponent* tc = GetComponent(g_selected, TextureComponent);
    if (index != (size_t)-1) {
        tc->id = index;
    }
    return tc->id;
}

static BOOL DrawTextureComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TextureComponent)) return FALSE;
    TextureComponent* tc = GetComponent(g_selected, TextureComponent);
    if (tc->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Texture");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Texture Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.texnames.size, (char**)g_selected.context->parent->assets.texnames.data, DropdownSelectTexture, NULL);
    return edited;
}

static size_t DropdownSelectAnimation(void* data, size_t index, BOOL cancel) {
    AnimationComponent* tc = GetComponent(g_selected, AnimationComponent);
    if (index != (size_t)-1) {
        tc->id = index;
    }
    return tc->id;
}

static BOOL DrawAnimationComponentUI(float width, float height) {
    if (!HasComponent(g_selected, AnimationComponent)) return FALSE;
    AnimationComponent* ac = GetComponent(g_selected, AnimationComponent);
    if (ac->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Animation");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Animation Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.animnames.size, (char**)g_selected.context->parent->assets.animnames.data, DropdownSelectAnimation, NULL);
    UIColumnHeader("Speed", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(ac->speed), 0.001f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Reset Time", LEFT_COLUMN_WIDTH);
    char fbuf[128] = { 0 };
    sprintf(fbuf, "%.3f", ac->time);
    if (UIButton(fbuf, width - LEFT_COLUMN_WIDTH - 20.0f)) ac->time = 0.0f;
    UIColumnHeader("Paused", LEFT_COLUMN_WIDTH);
    UICheckbox(&(ac->paused));
    UIColumnHeader("Looped", LEFT_COLUMN_WIDTH);
    UICheckbox(&(ac->loop));
    UIColumnHeader("Flipped", LEFT_COLUMN_WIDTH);
    UICheckbox(&(ac->flipped));
    UIColumnHeader("Flopped", LEFT_COLUMN_WIDTH);
    UICheckbox(&(ac->flopped));
    return edited;
}

static BOOL DrawListenerComponentUI(float width, float height) {
    if (!HasComponent(g_selected, ListenerComponent)) return FALSE;
    ListenerComponent* lc = GetComponent(g_selected, ListenerComponent);
    UIDividerLabeled(width, "Listener");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Enabled", LEFT_COLUMN_WIDTH);
    UICheckbox(&(lc->enabled));
    UIColumnHeader("Volume", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(lc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Decay", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(lc->decay), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    return edited;
}

static size_t DropdownSelectSound(void* data, size_t index, BOOL cancel) {
    SoundComponent* sc = GetComponent(g_selected, SoundComponent);
    if (index != (size_t)-1) {
        sc->id = index;
    }
    return sc->id;
}

static size_t DropdownSetSoundAudioCommand(void* data, size_t index, BOOL cancel) {
    SoundComponent* sc = GetComponent(g_selected, SoundComponent);
    if (index != (size_t)-1) {
        sc->command = (AudioCommand)index;
    }
    return (size_t)(sc->command);
}

static BOOL DrawSoundComponentUI(float width, float height) {
    if (!HasComponent(g_selected, SoundComponent)) return FALSE;
    SoundComponent* sc = GetComponent(g_selected, SoundComponent);
    if (sc->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Sound");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Sound Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.soundnames.size, (char**)g_selected.context->parent->assets.soundnames.data, DropdownSelectSound, NULL);
    UIColumnHeader("Volume", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(sc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Pitch", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(sc->pitch), 0.0f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Decay", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(sc->decay), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Audio Command", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, 5, (char**)g_audio_command_labels, DropdownSetSoundAudioCommand, NULL);
    return edited;
}

static size_t DropdownSelectMusic(void* data, size_t index, BOOL cancel) {
    MusicComponent* mc = GetComponent(g_selected, MusicComponent);
    if (index != (size_t)-1) {
        mc->id = index;
    }
    return mc->id;
}

static size_t DropdownSetMusicAudioCommand(void* data, size_t index, BOOL cancel) {
    MusicComponent* mc = GetComponent(g_selected, MusicComponent);
    if (index != (size_t)-1) {
        mc->command = (AudioCommand)index;
    }
    return (size_t)(mc->command);
}

static BOOL DrawMusicComponentUI(float width, float height) {
    if (!HasComponent(g_selected, MusicComponent)) return FALSE;
    MusicComponent* mc = GetComponent(g_selected, MusicComponent);
    if (mc->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Music");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Music Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.musicnames.size, (char**)g_selected.context->parent->assets.musicnames.data, DropdownSelectMusic, NULL);
    UIColumnHeader("Volume", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(mc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Pitch", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(mc->pitch), 0.0f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Decay", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(mc->decay), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Audio Command", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, 5, (char**)g_audio_command_labels, DropdownSetMusicAudioCommand, NULL);
    return edited;
}

static BOOL DrawTextComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TextComponent)) return FALSE;
    TextComponent* tc = GetComponent(g_selected, TextComponent);
    UIDividerLabeled(width, "Text");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Text Content", LEFT_COLUMN_WIDTH);
    edited |= UITextInput(NULL, tc->text, tc->capacity, width - LEFT_COLUMN_WIDTH - 20.0f, FALSE);
    UIColumnHeader("Alignment", LEFT_COLUMN_WIDTH);
    float boxw = 16.0f;
    float gapw = 2.0f;
    TextAlignment alignments[] = { TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT };
    UIMoveCursor(0, 2);
    for (size_t i = 0; i < 3; i++) {
        DrawRectangle(UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y, boxw, boxw, (Color){ 255, 255, 255, 150 });
        if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y, boxw, boxw})) {
            DrawRectangle(UIGetCursor().x + (i * (boxw + gapw)), UIGetCursor().y, boxw, boxw, (Color){ 255, 255, 255, 170 });
            if (InputButtonPressed(IK_MOUSELEFT)) {
                tc->alignment = alignments[i];
                edited = TRUE;
            }
        }
        if (tc->alignment == alignments[i])
            DrawCircle(UIGetCursor().x + (i * (boxw + gapw)) + (boxw/2.0f), UIGetCursor().y + (boxw/2.0f), 5, RED);
    }
    UIMoveCursor(10 - UIGetCursor().x, 18);
    UIColumnHeader("Opacity", LEFT_COLUMN_WIDTH);
    size_t alpha = tc->color.a;
    edited |= UIDragSize(&alpha, 0, 255, 1, width - LEFT_COLUMN_WIDTH - 20.0f);
    tc->color.a = (unsigned char)alpha;
    UIColumnHeader("Size", LEFT_COLUMN_WIDTH);
    edited |= UIDragFloat(&(tc->size), 0.0f, FLT_MAX, 0.05f, width - LEFT_COLUMN_WIDTH - 20.0f);
    size_t r = tc->color.r;
    size_t g = tc->color.g;
    size_t b = tc->color.b;
    edited |= UITriplet("rgb", UI_SIZES, &r, &g, &b, (UIMultiValue){ ._size = 0 }, (UIMultiValue){ ._size = 255 }, (UIMultiValue){ ._size = 1 }, (UIMultiValue){ ._size = 255 }, width);
    tc->color.r = (unsigned char)r;
    tc->color.g = (unsigned char)g;
    tc->color.b = (unsigned char)b;
    return edited;
}

static BOOL DrawCameraComponentUI(float width, float height) {
    if (!HasComponent(g_selected, CameraComponent)) return FALSE;
    CameraComponent* cc = GetComponent(g_selected, CameraComponent);
    float component_width = (width - LEFT_COLUMN_WIDTH - 20 - (2 * 16) - (1 * 10)) / 2.0f;
    UIDividerLabeled(width, "Camera");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Enabled", LEFT_COLUMN_WIDTH);
    UICheckbox(&(cc->enabled));
    UIColumnHeader("Offset", LEFT_COLUMN_WIDTH);
    DrawRectangle(UIGetCursor().x + 3, UIGetCursor().y + 1, 16, 18, RED);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x + 3, UIGetCursor().y + 1, 16, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        cc->offset.x = 0.0f;;
    }
    UIMoveCursor(7, 0);
    UIDrawText("x");
    UIMoveCursor(17 + LEFT_COLUMN_WIDTH, -20);
    edited |= UIDragFloat(&(cc->offset.x), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor(component_width + 31 + LEFT_COLUMN_WIDTH, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, BLUE);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        cc->offset.y = 0.0f;;
    }
    UIDrawText("y");
    UIMoveCursor(component_width + 42 + LEFT_COLUMN_WIDTH, -20);
    edited |= UIDragFloat(&(cc->offset.y), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIColumnHeader("Rotation", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(cc->rotation), -FLT_MAX, FLT_MAX, 0.1f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIColumnHeader("Zoom", LEFT_COLUMN_WIDTH);
    UIDragFloat(&(cc->zoom), 0.0001f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    return edited;
}

static size_t DropdownSetShape(void* data, size_t index, BOOL cancel) {
    ShapeComponent* sc = GetComponent(g_selected, ShapeComponent);
    if (index != (size_t)-1) {
        sc->type = (ShapeType)index;
    }
    return (size_t)(sc->type);
}

static BOOL DrawShapeComponentUI(float width, float height) {
    if (!HasComponent(g_selected, ShapeComponent)) return FALSE;
    ShapeComponent* sc = GetComponent(g_selected, ShapeComponent);
    UIDividerLabeled(width, "Shape");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Shape Type", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, 2, (char**)g_shape_labels, DropdownSetShape, NULL);
    UIColumnHeader("Opacity", LEFT_COLUMN_WIDTH);
    size_t alpha = sc->color.a;
    edited |= UIDragSize(&alpha, 0, 255, 1, width - LEFT_COLUMN_WIDTH - 20.0f);
    sc->color.a = (unsigned char)alpha;
    size_t r = sc->color.r;
    size_t g = sc->color.g;
    size_t b = sc->color.b;
    edited |= UITriplet("rgb", UI_SIZES, &r, &g, &b, (UIMultiValue){ ._size = 0 }, (UIMultiValue){ ._size = 255 }, (UIMultiValue){ ._size = 1 }, (UIMultiValue){ ._size = 255 }, width);
    sc->color.r = (unsigned char)r;
    sc->color.g = (unsigned char)g;
    sc->color.b = (unsigned char)b;
    return edited;
}

static size_t DropdownSelectScript(void* data, size_t index, BOOL cancel) {
    ScriptComponent* sc = GetComponent(g_selected, ScriptComponent);
    if (index != (size_t)-1) {
        sc->id = index;
    }
    return sc->id;
}

static BOOL DrawScriptComponentUI(float width, float height) {
    if (!HasComponent(g_selected, ScriptComponent)) return FALSE;
    ScriptComponent* sc = GetComponent(g_selected, ScriptComponent);
    if (sc->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Script");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Script Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->scripts.names.size, (char**)g_selected.context->parent->scripts.names.data, DropdownSelectScript, NULL);
    return edited;
}

static size_t DropdownSelectShader(void* data, size_t index, BOOL cancel) {
    ShaderComponent* sc = GetComponent(g_selected, ShaderComponent);
    if (index != (size_t)-1) {
        sc->id = index;
    }
    return sc->id;
}

static BOOL DrawShaderComponentUI(float width, float height) {
    if (!HasComponent(g_selected, ShaderComponent)) return FALSE;
    ShaderComponent* sc = GetComponent(g_selected, ShaderComponent);
    if (sc->id == (size_t)-1) return FALSE;
    UIDividerLabeled(width, "Shader");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIColumnHeader("Shader Asset", LEFT_COLUMN_WIDTH);
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.shadernames.size, (char**)g_selected.context->parent->assets.shadernames.data, DropdownSelectShader, NULL);
    return edited;
}

static void DrawEditPanel(float width, float height) {
    if (g_selected.id == INVALID_ENTITY || !IsActiveWorld(g_selected.context) || !HasComponent(g_selected, TagComponent)) {
        UISetCursor(width / 2.0f - (UITextWidth("No Selected Entity") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Selected Entity");
        return;
    }
    if (HasComponent(g_selected, EditUIComponent)) {
        EditUIComponent* euic = GetComponent(g_selected, EditUIComponent);
        if (euic->draw) {
            euic->draw(width, height);
            return;
        }
    }
    float panel_heights = 0;
    panel_heights += HasComponent(g_selected, TransformComponent) ? 95 : 0;
    panel_heights += HasComponent(g_selected, AnchorComponent) ? 85 : 0;
    panel_heights += HasComponent(g_selected, TextureComponent) ? 45 : 0;
    panel_heights += HasComponent(g_selected, AnimationComponent) ? 160 : 0;
    panel_heights += HasComponent(g_selected, ListenerComponent) ? 83 : 0;
    panel_heights += HasComponent(g_selected, SoundComponent) ? 123 : 0;
    panel_heights += HasComponent(g_selected, MusicComponent) ? 123 : 0;
    panel_heights += HasComponent(g_selected, TextComponent) ? 128 : 0;
    panel_heights += HasComponent(g_selected, CameraComponent) ? 105 : 0;
    panel_heights += HasComponent(g_selected, ShapeComponent) ? 90 : 0;
    panel_heights += HasComponent(g_selected, ScriptComponent) ? 45 : 0;
    panel_heights += HasComponent(g_selected, ShaderComponent) ? 45 : 0;
    UIMoveCursor(0, 35);
    if (HoveredPanel() && strcmp(HoveredPanel(), "Edit") == 0) {
        if (InputKeyPressed(IK_ENTER)) g_scrolldiff = 0.0f;
        g_scrolldiff -= 18.0f * GetMouseWheelMove();
    }
    if (g_scrolldiff > panel_heights - height + 45) g_scrolldiff = panel_heights - height + 45;
    if (g_scrolldiff < 0) g_scrolldiff = 0.0f;
    UIMoveCursor(0, -g_scrolldiff);
    DrawTransformComponentUI(width, height);
    DrawAnchorComponentUI(width, height);
    DrawTextureComponentUI(width, height);
    DrawAnimationComponentUI(width, height);
    DrawListenerComponentUI(width, height);
    DrawSoundComponentUI(width, height);
    DrawMusicComponentUI(width, height);
    DrawTextComponentUI(width, height);
    DrawCameraComponentUI(width, height);
    DrawShapeComponentUI(width, height);
    DrawScriptComponentUI(width, height);
    DrawShaderComponentUI(width, height);
    UISetCursor(10, 10);
    DrawRectangle(0, 0, width, 45, MappedColor(PANEL_BG_COLOR));
    DrawTagComponentUI(width, height);
}

Panel GenerateEditPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Edit");
	p.draw = DrawEditPanel;
	return p;
}

void SelectEntity(Entity e) {
    g_selected = e;
}

Entity SelectedEntity() {
    return g_selected;
}

void SetHoveredEntity(Entity e) {
    g_hovered = e;
}

Entity HoveredEntity() {
    return g_hovered;
}
