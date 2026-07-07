#include "assets.h"
#include "core/application.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ui/ui.h"
#include <raymath.h>

static float g_canvas_height = 230;
static BOOL g_textures_opened = FALSE;
static BOOL g_sounds_opened = FALSE;
static BOOL g_music_opened = FALSE;
static BOOL g_animations_opened = FALSE;
static size_t g_selected_asset = (size_t)-1;
static int g_selected_asset_type = 0;
static float g_scrolldiff = 0.0f;
static BOOL g_resize_canvas = FALSE;

static void DrawAssetsPanel(float width, float height) {
    if (HoveredPanel() && strcmp(HoveredPanel(), "Assets") == 0) {
        if (InputKeyPressed(IK_ENTER)) g_scrolldiff = 0.0f;
        g_scrolldiff -= 18.0f * GetMouseWheelMove();
    }
    Scene* scene = GetActiveScene();
    UISetCursor(10, g_canvas_height + 50);
    int numlines = 4;
    if (g_textures_opened) numlines += scene->assets.textures.size;
    if (g_animations_opened) numlines += scene->assets.animations.size;
    if (g_sounds_opened) numlines += scene->assets.sounds.size;
    if (g_music_opened) numlines += scene->assets.musics.size;
    if (g_selected_asset != (size_t)-1) {
        if (g_selected_asset_type == 1) {
            numlines += 5;
        } else {
            numlines += 2;
        }
    }
    if (g_scrolldiff > numlines * LINE_HEIGHT - height + g_canvas_height + 90) g_scrolldiff = numlines * LINE_HEIGHT - height + g_canvas_height + 90;
    if (g_scrolldiff < 0) g_scrolldiff = 0.0f;
    UIMoveCursor(0, 10 - g_scrolldiff);
    UIMoveCursor(20, 0);
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 20 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x + 10, 20})) {
        DrawRectangle(UIGetCursor().x - 20, UIGetCursor().y, width - UIGetCursor().x + 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) g_textures_opened = !g_textures_opened;
    }
    if (g_textures_opened) {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f, UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            MappedColor(UI_TEXT_COLOR));
    } else {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f },
            MappedColor(UI_TEXT_COLOR));
    }
    UIDrawText("Textures");
    if (g_textures_opened) {
        float ystickorig = UIGetCursor().y + 3;
        for (size_t i = 0; i < scene->assets.textures.size; i++) {
            DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
            UIMoveCursor(25, 0);
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
                if (InputButtonPressed(IK_MOUSELEFT)) {
                    if (g_selected_asset_type == 0 && g_selected_asset == i) {
                        g_selected_asset = (size_t)-1;
                    } else {
                        g_selected_asset = i;
                        g_selected_asset_type = 0;
                    }
                }
            }
            if (g_selected_asset == i && g_selected_asset_type == 0) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
            }
            UIDrawText("%s", scene->assets.texnames.data[i]);
            if (i == scene->assets.textures.size - 1) {
                DrawRectangle(UIGetCursor().x + 10, ystickorig, 1, UIGetCursor().y - ystickorig - 8, MappedColor(UI_TEXT_COLOR));
            }
            if (g_selected_asset == i && g_selected_asset_type == 0) {
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 3, 1, 8, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("path: \"%s\"", scene->assets.texpaths.data[i]);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("size: %dx%d pixels", (int)scene->assets.textures.data[i].width, (int)scene->assets.textures.data[i].height);
            }
        }
    }
    UIMoveCursor(20, 0);
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 20 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x + 10, 20})) {
        DrawRectangle(UIGetCursor().x - 20, UIGetCursor().y, width - UIGetCursor().x + 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) g_animations_opened = !g_animations_opened;
    }
    if (g_animations_opened) {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f, UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            MappedColor(UI_TEXT_COLOR));
    } else {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f },
            MappedColor(UI_TEXT_COLOR));
    }
    UIDrawText("Animations");
    if (g_animations_opened) {
        float ystickorig = UIGetCursor().y + 3;
        for (size_t i = 0; i < scene->assets.animations.size; i++) {
            DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
            UIMoveCursor(25, 0);
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
                if (InputButtonPressed(IK_MOUSELEFT)) {
                    if (g_selected_asset_type == 1 && g_selected_asset == i) {
                        g_selected_asset = (size_t)-1;
                    } else {
                        g_selected_asset = i;
                        g_selected_asset_type = 1;
                    }
                }
            }
            if (g_selected_asset == i && g_selected_asset_type == 1) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
            }
            UIDrawText("%s", scene->assets.animnames.data[i]);
            if (i == scene->assets.animations.size - 1) {
                DrawRectangle(UIGetCursor().x + 10, ystickorig, 1, UIGetCursor().y - ystickorig - 8, MappedColor(UI_TEXT_COLOR));
            }
            if (g_selected_asset == i && g_selected_asset_type == 1) {
                Animation anim = scene->assets.animations.data[i];
                EZ_ASSERT(anim.sheet == (size_t)-1 || anim.sheet < scene->assets.textures.size, "Invalid sheet reference");
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 3, 1, 8, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("Sprite Sheet: \"%s\"", anim.sheet == (size_t)-1 ? "None" : scene->assets.texnames.data[anim.sheet]);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("frame size: %dx%d pixels", (int)anim.width, (int)anim.height);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("frame count: %d", (int)anim.frames);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("origin: %.1fx%.1f", anim.origin.x, anim.origin.y);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("FPS: %.3f", anim.fps);
            }
        }
    }
    UIMoveCursor(20, 0);
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 20 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x + 10, 20})) {
        DrawRectangle(UIGetCursor().x - 20, UIGetCursor().y, width - UIGetCursor().x + 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) g_sounds_opened = !g_sounds_opened;
    }
    if (g_sounds_opened) {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f, UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            MappedColor(UI_TEXT_COLOR));
    } else {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f },
            MappedColor(UI_TEXT_COLOR));
    }
    UIDrawText("Sounds");
    if (g_sounds_opened) {
        float ystickorig = UIGetCursor().y + 3;
        for (size_t i = 0; i < scene->assets.sounds.size; i++) {
            DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
            UIMoveCursor(25, 0);
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
                if (InputButtonPressed(IK_MOUSELEFT)) {
                    if (g_selected_asset_type == 2 && g_selected_asset == i) {
                        g_selected_asset = (size_t)-1;
                    } else {
                        g_selected_asset = i;
                        g_selected_asset_type = 2;
                    }
                }
            }
            if (g_selected_asset == i && g_selected_asset_type == 2) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
            }
            UIDrawText("%s", scene->assets.soundnames.data[i]);
            if (i == scene->assets.sounds.size - 1) {
                DrawRectangle(UIGetCursor().x + 10, ystickorig, 1, UIGetCursor().y - ystickorig - 8, MappedColor(UI_TEXT_COLOR));
            }
            if (g_selected_asset == i && g_selected_asset_type == 2) {
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 3, 1, 8, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("path: \"%s\"", scene->assets.soundpaths.data[i]);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("frames: %d", (int)scene->assets.sounds.data[i].frameCount);
            }
        }
    }
    UIMoveCursor(20, 0);
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 20 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x + 10, 20})) {
        DrawRectangle(UIGetCursor().x - 20, UIGetCursor().y, width - UIGetCursor().x + 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) g_music_opened = !g_music_opened;
    }
    if (g_music_opened) {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f, UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            MappedColor(UI_TEXT_COLOR));
    } else {
        DrawTriangle(
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f - (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f - (10.0f / 2.0f), UIGetCursor().y + 10.0f + (10.0f / 2.0f) },
            (Vector2){ UIGetCursor().x - 10.0f + (10.0f / 2.0f), UIGetCursor().y + 10.0f },
            MappedColor(UI_TEXT_COLOR));
    }
    UIDrawText("Music Streams");
    if (g_music_opened) {
        float ystickorig = UIGetCursor().y + 3;
        for (size_t i = 0; i < scene->assets.musics.size; i++) {
            DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
            UIMoveCursor(25, 0);
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
                if (InputButtonPressed(IK_MOUSELEFT)) {
                    if (g_selected_asset_type == 3 && g_selected_asset == i) {
                        g_selected_asset = (size_t)-1;
                    } else {
                        g_selected_asset = i;
                        g_selected_asset_type = 3;
                    }
                }
            }
            if (g_selected_asset == i && g_selected_asset_type == 3) {
                DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
            }
            UIDrawText("%s", scene->assets.musicnames.data[i]);
            if (i == scene->assets.musics.size - 1) {
                DrawRectangle(UIGetCursor().x + 10, ystickorig, 1, UIGetCursor().y - ystickorig - 8, MappedColor(UI_TEXT_COLOR));
            }
            if (g_selected_asset == i && g_selected_asset_type == 3) {
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 3, 1, 8, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("path: \"%s\"", scene->assets.musicpaths.data[i]);
                UIMoveCursor(20, 0);
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y - 9, 1, 20, MappedColor(UI_TEXT_COLOR));
                DrawRectangle(UIGetCursor().x + 10, UIGetCursor().y + 11, 10, 1, MappedColor(UI_TEXT_COLOR));
                UIMoveCursor(25, 0);
                UIDrawText("frames: %d", (int)scene->assets.musics.data[i].frameCount);
            }
        }
    }
    UISetCursor(10, 10);
    DrawRectangle(0, 0, width, g_canvas_height + 55, MappedColor(PANEL_BG_COLOR));
    UIDrawText("Loaded Assets");
    UIDivider(width - 20);
    DrawRectangle(UIGetCursor().x, UIGetCursor().y, width - 20, g_canvas_height, MappedColor(PANEL_ASSET_BORDER_COLOR));
    DrawRectangle(UIGetCursor().x + 2, UIGetCursor().y + 2, width - 20 - 4, g_canvas_height - 4, MappedColor(PANEL_ASSET_BG_COLOR));
    DrawTriangle(
        (Vector2){ UIGetCursor().x + width - 20, UIGetCursor().y + g_canvas_height - 15 },
        (Vector2){ UIGetCursor().x + width - 35, UIGetCursor().y + g_canvas_height },
        (Vector2){ UIGetCursor().x + width - 20, UIGetCursor().y + g_canvas_height },
        MappedColor(PANEL_ASSET_BORDER_COLOR));
    if (CheckCollisionPointTriangle(
        Vector2Subtract(GetMousePosition(), UIGetPosition()),
        (Vector2){ UIGetCursor().x + width - 20, UIGetCursor().y + g_canvas_height - 15 },
        (Vector2){ UIGetCursor().x + width - 35, UIGetCursor().y + g_canvas_height },
        (Vector2){ UIGetCursor().x + width - 20, UIGetCursor().y + g_canvas_height })) {
        if (InputButtonPressed(IK_MOUSELEFT)) g_resize_canvas = TRUE;
    }
    if (InputButtonReleased(IK_MOUSELEFT)) g_resize_canvas = FALSE;
    if (g_resize_canvas) {
        g_canvas_height += GetMouseDelta().y;
        if (g_canvas_height < 30.0f) g_canvas_height = 30.0f;
    }
    if (g_selected_asset != (size_t)-1) {
        if (g_selected_asset_type == 0 && g_selected_asset < scene->assets.textures.size) {
            Texture2D texture = scene->assets.textures.data[g_selected_asset];
            float wratio = texture.width / (width - 20 - 4);
            float hratio = texture.height / (g_canvas_height - 4);
            if (wratio > hratio) {
                DrawTexturePro(
                    texture,
                    (Rectangle){ 0, 0, texture.width, texture.height },
                    (Rectangle){ 
                        UIGetCursor().x + 2,
                        UIGetCursor().y + 2 + ((g_canvas_height - 4) / 2.0f) - ((texture.height / wratio) / 2.0f),
                        width - 20 - 4,
                        texture.height / wratio },
                    (Vector2){ 0, 0 }, 0, WHITE
                );
            } else {
                DrawTexturePro(
                    texture,
                    (Rectangle){ 0, 0, texture.width, texture.height },
                    (Rectangle){ 
                        UIGetCursor().x + 2 + ((width - 20 - 4) / 2.0f) - ((texture.width / hratio) / 2.0f),
                        UIGetCursor().y + 2,
                        texture.width / hratio,
                        g_canvas_height - 4 },
                    (Vector2){ 0, 0 }, 0, WHITE
                );
            }
        } else {
            UISetCursor((width / 2.0f) - (UITextWidth("No Asset Selected!") / 2.0f), 40 + g_canvas_height / 2.0f);
            UIDrawText("No Asset Selected!");
        }
    } else {
        UISetCursor((width / 2.0f) - (UITextWidth("No Asset Selected!") / 2.0f), 40 + g_canvas_height / 2.0f);
        UIDrawText("No Asset Selected!");
    }
}

Panel GenerateAssetsPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Assets");
	p.draw = DrawAssetsPanel;
	return p;
}
