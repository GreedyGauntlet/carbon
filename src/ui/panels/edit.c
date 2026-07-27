#include "edit.h"
#include "data/definitions.h"
#include "ui/ui.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "core/application.h"
#include "core/world.h"
#include "util/logger.h"
#include "data/colors.h"
#include "data/input.h"
#include <float.h>
#include <raymath.h>

#define LEFT_COLUMN_WIDTH 150

static Entity g_selected = { 0 };
static const char* g_audio_command_labels[] = { "Send Command", "Play", "Pause", "Resume", "Stop" };
static const char* g_shape_labels[] = { "Rectangle", "Circle" };
static float g_scrolldiff = 0.0f;

static void DrawComponentTitle(float width, const char* title) {
    float tratio = 2.0f * UITextWidth(title) / (width - 20.0f);
    float thresh = (width - 20.0f) * (1.0f - tratio);
    DrawRectangleGradientH(UIGetCursor().x + thresh, UIGetCursor().y + (LINE_HEIGHT/2.0f) - 1, width - 20.0f - thresh, 2, MappedColor(UI_DIVIDER_COLOR), (Color){ 0, 0, 0, 0 });
    UIDivider(thresh);
    UIMoveCursor(width - UITextWidth(title) - 30, -20);
    UIDrawItalicText(title);
}

static void DrawTagComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TagComponent)) return;
    TagComponent* tc = GetComponent(g_selected, TagComponent);
    UIMoveCursor(0, 5);
    DrawRectangleGradientH(UIGetCursor().x - 3, UIGetCursor().y - 3, width - 14, 26, (Color){ 255, 255, 255, 190 }, (Color){ 0 });
    UIDrawText(tc->tag);
}

static BOOL DrawTransformComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TransformComponent)) return FALSE;
    DrawComponentTitle(width, "Transform");
    TransformComponent* tc = GetComponent(g_selected, TransformComponent);
    float component_width = (width - 20 - (3 * 16) - (2 * 10)) / 3.0f;
    BOOL edited = FALSE;
    UIMoveCursor(5, 5);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, RED);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->translation.x = 0.0f;
    }
    UIDrawText("x");
    UIMoveCursor(17, -20);
    edited |= UIDragFloat(&(tc->translation.x), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor(component_width + 31, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, GREEN);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->translation.y = 0.0f;
    }
    UIDrawText("y");
    UIMoveCursor(component_width + 42, -20);
    edited |= UIDragFloat(&(tc->translation.y), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor((2*component_width) + 56, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, BLUE);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->translation.z = 0.0f;
    }
    UIDrawText("z");
    UIMoveCursor((2*component_width) + 67, -20);
    edited |= UIDragFloat(&(tc->translation.z), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor(5, 5);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, GOLD);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->scale.x = 100.0f;
    }
    UIDrawText("w");
    UIMoveCursor(17, -20);
    edited |= UIDragFloat(&(tc->scale.x), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor(component_width + 31, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, MAGENTA);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->scale.y = 100.0f;
    }
    UIDrawText("h");
    UIMoveCursor(component_width + 42, -20);
    edited |= UIDragFloat(&(tc->scale.y), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    UIMoveCursor((2*component_width) + 56, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, SKYBLUE);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        tc->rotation = 0.0f;
    }
    UIDrawText("r");
    UIMoveCursor((2*component_width) + 67, -20);
    edited |= UIDragFloat(&(tc->rotation), -FLT_MAX, FLT_MAX, 0.1f, component_width);
    return edited;
}

static BOOL DrawAnchorComponentUI(float width, float height) {
    if (!HasComponent(g_selected, AnchorComponent)) return FALSE;
    DrawComponentTitle(width, "Anchor");
    AnchorComponent* ac = GetComponent(g_selected, AnchorComponent);
    UIMoveCursor(0, 25);
    UIDrawText("Anchor Alignment");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y - 25, 2, 65, (Color){ 255, 255, 255, 130 });
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
    DrawComponentTitle(width, "Texture");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Texture Asset");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    DrawComponentTitle(width, "Animation");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Animation Asset");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.animnames.size, (char**)g_selected.context->parent->assets.animnames.data, DropdownSelectAnimation, NULL);
    UIDrawText("Speed");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(ac->speed), 0.001f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Reset Time");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    char fbuf[128] = { 0 };
    sprintf(fbuf, "%.3f", ac->time);
    if (UIButton(fbuf, width - LEFT_COLUMN_WIDTH - 20.0f)) ac->time = 0.0f;
    UIDrawText("Paused");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(ac->paused));
    UIDrawText("Looped");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(ac->loop));
    UIDrawText("Flipped");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(ac->flipped));
    UIDrawText("Flopped");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(ac->flopped));
    return edited;
}

static BOOL DrawListenerComponentUI(float width, float height) {
    if (!HasComponent(g_selected, ListenerComponent)) return FALSE;
    ListenerComponent* lc = GetComponent(g_selected, ListenerComponent);
    DrawComponentTitle(width, "Listener");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Enabled");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(lc->enabled));
    UIDrawText("Volume");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(lc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Decay");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    DrawComponentTitle(width, "Sound");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Sound Asset");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.soundnames.size, (char**)g_selected.context->parent->assets.soundnames.data, DropdownSelectSound, NULL);
    UIDrawText("Volume");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(sc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Pitch");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(sc->pitch), 0.0f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Decay");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(sc->decay), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Audio Command");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    DrawComponentTitle(width, "Music");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Music Asset");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->assets.musicnames.size, (char**)g_selected.context->parent->assets.musicnames.data, DropdownSelectMusic, NULL);
    UIDrawText("Volume");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(mc->volume), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Pitch");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(mc->pitch), 0.0f, FLT_MAX, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Decay");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(mc->decay), 0.0f, 1.0f, 0.001f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Audio Command");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, 5, (char**)g_audio_command_labels, DropdownSetMusicAudioCommand, NULL);
    return edited;
}

static BOOL DrawTextComponentUI(float width, float height) {
    if (!HasComponent(g_selected, TextComponent)) return FALSE;
    TextComponent* tc = GetComponent(g_selected, TextComponent);
    DrawComponentTitle(width, "Text");
    BOOL edited = FALSE;
    float component_width = (width - 20 - (3 * 16) - (2 * 10)) / 3.0f;
    UIMoveCursor(0, 2);
    UIDrawText("Text Content");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    edited |= UITextInput(NULL, tc->text, tc->capacity, width - LEFT_COLUMN_WIDTH - 20.0f, FALSE);
    UIDrawText("Alignment");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    UIDrawText("Opacity");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    size_t alpha = tc->color.a;
    edited |= UIDragSize(&alpha, 0, 255, 1, width - LEFT_COLUMN_WIDTH - 20.0f);
    tc->color.a = (unsigned char)alpha;
    UIDrawText("Size");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    edited |= UIDragFloat(&(tc->size), 0.0f, FLT_MAX, 0.05f, width - LEFT_COLUMN_WIDTH - 20.0f);
    size_t r = tc->color.r;
    size_t g = tc->color.g;
    size_t b = tc->color.b;
    UIMoveCursor(5, 10);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, RED);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        r = 255;
    }
    UIDrawText("r");
    UIMoveCursor(17, -20);
    edited |= UIDragSize(&r, 0, 255, 1, component_width);
    UIMoveCursor(component_width + 31, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, GREEN);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        g = 255;
    }
    UIDrawText("g");
    UIMoveCursor(component_width + 42, -20);
    edited |= UIDragSize(&g, 0, 255, 1, component_width);
    UIMoveCursor((2*component_width) + 56, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, BLUE);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        b = 255;
    }
    UIDrawText("b");
    UIMoveCursor((2*component_width) + 67, -20);
    edited |= UIDragSize(&b, 0, 255, 1, component_width);
    tc->color.r = (unsigned char)r;
    tc->color.g = (unsigned char)g;
    tc->color.b = (unsigned char)b;
    return edited;
}

static BOOL DrawCameraComponentUI(float width, float height) {
    if (!HasComponent(g_selected, CameraComponent)) return FALSE;
    CameraComponent* cc = GetComponent(g_selected, CameraComponent);
    float component_width = (width - LEFT_COLUMN_WIDTH - 20 - (2 * 16) - (1 * 10)) / 2.0f;
    DrawComponentTitle(width, "Camera");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Enabled");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UICheckbox(&(cc->enabled));
    UIDrawText("Offset");
    UIMoveCursor(LEFT_COLUMN_WIDTH - 2, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 8, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    UIDrawText("Rotation");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDragFloat(&(cc->rotation), -FLT_MAX, FLT_MAX, 0.1f, width - LEFT_COLUMN_WIDTH - 20.0f);
    UIDrawText("Zoom");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
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
    DrawComponentTitle(width, "Shape");
    BOOL edited = FALSE;
    float component_width = (width - 20 - (3 * 16) - (2 * 10)) / 3.0f;
    UIMoveCursor(0, 2);
    UIDrawText("Shape Type");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, 2, (char**)g_shape_labels, DropdownSetShape, NULL);
    UIDrawText("Opacity");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    size_t alpha = sc->color.a;
    edited |= UIDragSize(&alpha, 0, 255, 1, width - LEFT_COLUMN_WIDTH - 20.0f);
    sc->color.a = (unsigned char)alpha;
    size_t r = sc->color.r;
    size_t g = sc->color.g;
    size_t b = sc->color.b;
    UIMoveCursor(5, 10);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, RED);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        r = 255;
    }
    UIDrawText("r");
    UIMoveCursor(17, -20);
    edited |= UIDragSize(&r, 0, 255, 1, component_width);
    UIMoveCursor(component_width + 31, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, GREEN);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        g = 255;
    }
    UIDrawText("g");
    UIMoveCursor(component_width + 42, -20);
    edited |= UIDragSize(&g, 0, 255, 1, component_width);
    UIMoveCursor((2*component_width) + 56, -20);
    DrawRectangle(UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18, BLUE);
    if (CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){UIGetCursor().x - 5, UIGetCursor().y + 1, 20, 18}) &&
        InputButtonPressed(IK_MOUSELEFT)) {
        b = 255;
    }
    UIDrawText("b");
    UIMoveCursor((2*component_width) + 67, -20);
    edited |= UIDragSize(&b, 0, 255, 1, component_width);
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
    DrawComponentTitle(width, "Script");
    BOOL edited = FALSE;
    UIMoveCursor(0, 2);
    UIDrawText("Script Asset");
    UIMoveCursor(LEFT_COLUMN_WIDTH, -LINE_HEIGHT);
    DrawRectangle(UIGetCursor().x - 10, UIGetCursor().y, 2, 20, (Color){ 255, 255, 255, 130 });
    UIDropdownMenu(width - LEFT_COLUMN_WIDTH - 20, g_selected.context->parent->scripts.names.size, (char**)g_selected.context->parent->scripts.names.data, DropdownSelectScript, NULL);
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
