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
    UISetCursor(width - 38, 10);
    if (UIButton("+", 0)) {
        // TODO:
        logerror("Adding components dynamically have not been implemented yet!");
    }
    UIMoveCursor(0, 5);
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
    BOOL edited = FALSE;
    float boxw = 20.0f;
    float gapw = 2.0f;
    ViewportAnchor anchors[] = { TL_ANCHOR, ML_ANCHOR, BL_ANCHOR, TM_ANCHOR, CENTER_ANCHOR, BM_ANCHOR, TR_ANCHOR, MR_ANCHOR, BR_ANCHOR };
    const char* labels[] = { "TOP LEFT", "MIDDLE LEFT", "BOTTOM LEFT", "TOP MIDDLE", "CENTER", "BOTTOM MIDDLE", "TOP RIGHT", "MIDDLE RIGHT", "BOTTOM RIGHT" };
    size_t selected = 0;
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
            if (ac->anchor == anchors[i * 3 + j]) {
                selected = i * 3 + j;
                DrawCircle(UIGetCursor().x + (i * (boxw + gapw)) + (boxw/2.0f), UIGetCursor().y + (j * (boxw + gapw)) + (boxw/2.0f), 5, RED);
            }
        }
    }
    UIMoveCursor((boxw + gapw) * 3 - gapw + (width - (boxw + gapw) * 3 - gapw)/2.0f - (UITextWidth("Current Entity Anchor:")) / 2.0f, 10);
    UIDrawText("Current Entity Anchor:");
    UIMoveCursor((boxw + gapw) * 3 - gapw + (width - (boxw + gapw) * 3 - gapw)/2.0f - (UITextWidth(labels[selected])) / 2.0f, 0);
    UIDrawText(labels[selected]);
    UIMoveCursor(0, 15);
    return edited;
}

static size_t DropdownSelectTexture(void* data, size_t index) {
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

static size_t DropdownSelectAnimation(void* data, size_t index) {
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

static size_t DropdownSelectSound(void* data, size_t index) {
    SoundComponent* sc = GetComponent(g_selected, SoundComponent);
    if (index != (size_t)-1) {
        sc->id = index;
    }
    return sc->id;
}

static size_t DropdownSetSoundAudioCommand(void* data, size_t index) {
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

static size_t DropdownSelectMusic(void* data, size_t index) {
    MusicComponent* mc = GetComponent(g_selected, MusicComponent);
    if (index != (size_t)-1) {
        mc->id = index;
    }
    return mc->id;
}

static size_t DropdownSetMusicAudioCommand(void* data, size_t index) {
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

static void DrawEditPanel(float width, float height) {
    if (g_selected.id == INVALID_ENTITY || !IsActiveWorld(g_selected.context) || !HasComponent(g_selected, TagComponent)) {
        UISetCursor(width / 2.0f - (UITextWidth("No Selected Entity") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Selected Entity");
        return;
    }
    DrawTagComponentUI(width, height);
    DrawTransformComponentUI(width, height);
    DrawAnchorComponentUI(width, height);
    DrawTextureComponentUI(width, height);
    DrawAnimationComponentUI(width, height);
    DrawListenerComponentUI(width, height);
    DrawSoundComponentUI(width, height);
    DrawMusicComponentUI(width, height);
    // -- text --------------------------------------
    // text: [    text     ]
    // alightnment [ alightnment]
    // size: [ size ]
    // [ r ] [ g ] [b ] [a] 
    // -- camera ------------------------------------
    // enabled: []
    // [ x] [ y]
    // [ rotatopnm] ( dial with line that rotates!)
    // zoom : [ zoom ]
    // -- shape ------------------------------------- 
    // type [ type] 
    // [ r ] [ g ] [b ] [a] 
    // -- script ------------------------------------
    // script [    asset      ] [reload]
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
