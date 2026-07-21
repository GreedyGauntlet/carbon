#include "viewport.h"
#include "core/application.h"
#include "core/scene.h"
#include "core/binds.h"
#include "core/config.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ui/ui.h"
#include "ui/popup.h"
#include "systems/draw.h"
#include <raymath.h>

static Vector2 g_viewport_slice = { 0 };
static Vector2 g_viewport_position = { 0 };
static RenderTexture2D g_viewport_target = { 0 };
static BOOL g_rfocused = FALSE;
static BOOL g_zfocused = FALSE;
static BOOL g_show_hints = FALSE;

static BOOL DrawFullscreenButton(float x, float y) {
    const float offset = 5.0f;
    const float inner = 4.0f;
    const float thickness = 11.0f;
    DrawRectangle(x + offset, y + offset, 25 - (offset * 2), 25 - (offset * 2), IsFullScreen() ? MappedColor(UI_BTN_DISABLED) : MappedColor(UI_BTN_COLOR));
    DrawRectangle(x + offset + ((25 - (offset * 2)) / 2.0f) - (inner / 2.0f), y + offset, inner + 1, 25 - (offset * 2), MappedColor(PANEL_NB_COLOR));
    DrawRectangle(x + offset, y + offset + ((25 - (offset * 2)) / 2.0f) - (inner / 2.0f), 25 - (offset * 2), inner + 1, MappedColor(PANEL_NB_COLOR));
    DrawRectangle(x + offset + ((25 - (offset * 2)) / 2.0f) - (thickness / 2.0f), y + offset + ((25 - (offset * 2)) / 2.0f) - (thickness / 2.0f), thickness, thickness, MappedColor(PANEL_NB_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static void ToggleHints() {
    g_show_hints = !g_show_hints;
}

static BOOL DrawSettingsButton(float x, float y) {
    const float outer = 6.0f;
    const float inner = 4.0f;
    const float cogw = 4.0f;
    const float cogl = 18.0f;
    DrawCircle(x + 12.5f, y + 12.5f, outer, MappedColor(UI_BTN_COLOR));
    DrawRectangle(x + 12.5f - (cogw / 2.0f), y + 12.5f - (cogl / 2.0f), cogw, cogl, MappedColor(UI_BTN_COLOR));
    DrawRectanglePro(
        (Rectangle){ x + 12.0f, y + 12.0f, cogw, cogl },
        (Vector2){ cogw / 2.0f, cogl / 2.0f }, 0.0f, MappedColor(UI_BTN_COLOR));
    DrawRectanglePro(
        (Rectangle){ x + 12.0f, y + 12.0f, cogw, cogl },
        (Vector2){ cogw / 2.0f, cogl / 2.0f }, 60.0f, MappedColor(UI_BTN_COLOR));
    DrawRectanglePro(
        (Rectangle){ x + 12.0f, y + 12.0f, cogw, cogl },
        (Vector2){ cogw / 2.0f, cogl / 2.0f }, 120.0f, MappedColor(UI_BTN_COLOR));
    DrawCircle(x + 12.5f, y + 12.5f, inner, MappedColor(PANEL_NB_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static BOOL DrawResetButton(float x, float y) {
    const float outer = 9.0f;
    const float inner = 6.0f;
    const float tri = 9.0f;
    DrawCircle(x + 12.5f, y + 12.5f, outer, MappedColor(UI_BTN_COLOR));
    DrawRectangle(x + 12.5f, y, 12.5f, 12.5f, MappedColor(PANEL_NB_COLOR));
    DrawCircle(x + 12.5f, y + 12.5f, inner, MappedColor(PANEL_NB_COLOR));
    DrawTriangle(
        (Vector2){ x + 18.75f, y + 12.5f - (tri / 2.0f) },
        (Vector2){ x + 18.75f - (tri / 2.0f), y + 12.5f },
        (Vector2){ x + 18.75f + (tri / 2.0f), y + 12.5f },
        MappedColor(UI_BTN_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static BOOL DrawPlayButton(float x, float y) {
    const float width = 13.0f;
    const float height = 15.0f;
    DrawTriangle(
        (Vector2){ x + 12.5f - (width / 2.0f), y + 12.5f - (height / 2.0f) },
        (Vector2){ x + 12.5f - (width / 2.0f), y + 12.5f + (height / 2.0f) },
        (Vector2){ x + 12.5f + (width / 2.0f), y + 12.5f },
        MappedColor(UI_BTN_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static BOOL DrawPauseButton(float x, float y) {
    const float width = 4.0f;
    const float height = 15.0f;
    const float space = 3.0f;
    DrawRectangle(x + 12.5f - (space / 2.0f) - width, y + 12.5f - (height / 2.0f), width, height, MappedColor(UI_BTN_COLOR));
    DrawRectangle(x + 12.5f + (space / 2.0f), y + 12.5f - (height / 2.0f), width, height, MappedColor(UI_BTN_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static BOOL DrawFastForwardButton(float x, float y) {
    const float width = 12.0f;
    const float height = 15.0f;
    const float space = 8.0f;
    DrawTriangle(
        (Vector2){ x + 12.5f - (width / 2.0f) - (space / 2.0f), y + 12.5f - (height / 2.0f) },
        (Vector2){ x + 12.5f - (width / 2.0f) - (space / 2.0f), y + 12.5f + (height / 2.0f) },
        (Vector2){ x + 12.5f + (width / 2.0f) - (space / 2.0f), y + 12.5f },
        IsFast() ? MappedColor(UI_BTN_DISABLED) : MappedColor(UI_BTN_COLOR));
    DrawTriangle(
        (Vector2){ x + 12.5f - (width / 2.0f) + (space / 2.0f), y + 12.5f - (height / 2.0f) },
        (Vector2){ x + 12.5f - (width / 2.0f) + (space / 2.0f), y + 12.5f + (height / 2.0f) },
        (Vector2){ x + 12.5f + (width / 2.0f) + (space / 2.0f), y + 12.5f },
        IsFast() ? MappedColor(UI_BTN_DISABLED) : MappedColor(UI_BTN_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static BOOL DrawStepButton(float x, float y) {
    const float width = 13.0f;
    const float height = 15.0f;
    const float bar = 3.0f;
    const float space = 3.0f;
    DrawTriangle(
        (Vector2){ x + 12.5f - (width / 2.0f) + space, y + 12.5f - (height / 2.0f) },
        (Vector2){ x + 12.5f - (width / 2.0f) + space, y + 12.5f + (height / 2.0f) },
        (Vector2){ x + 12.5f + (width / 2.0f) + space, y + 12.5f },
        MappedColor(UI_BTN_COLOR));
    DrawRectangle(x + 12.5f - (width / 2.0f) - bar, y + 12.5f - (height / 2.0f), bar, height, MappedColor(UI_BTN_COLOR));
    if (InputButtonPressed(IK_MOUSELEFT) &&
        GetMouseX() > x + UIGetPosition().x &&
        GetMouseX() < x + UIGetPosition().x + 25 &&
        GetMouseY() > y + UIGetPosition().y &&
        GetMouseY() < y + UIGetPosition().y + 25) return TRUE;
    return FALSE;
}

static void ResizeViewportTarget() {
    if (g_viewport_target.texture.width != GetScreenWidth() || g_viewport_target.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(g_viewport_target);
        g_viewport_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }
}

static void DrawViewportPanel(float width, float height) {
    ClearBackground(BLACK);
    DrawRectangle(0, 0, width, 25, MappedColor(PANEL_NB_COLOR));
    DrawRectangle(0, 25, width, 1, MappedColor(PANEL_DIVIDER_COLOR));
    if (DrawFullscreenButton(0, 0)) ToggleFullScreen();
    if (DrawSettingsButton(width - 25, 0)) UIPopup(GenerateEditorConfigPopup());
    if (DrawResetButton(width / 2.0f - 50, 0) && GetActiveScene()) ResetScene(GetActiveScene());
    if (Playing()) {
        if (DrawPauseButton(width / 2.0f - 25, 0)) Pause();
    } else {
        if (DrawPlayButton(width / 2.0f - 25, 0)) Resume();
    }
    if (DrawFastForwardButton(width / 2.0f + 0, 0)) ToggleFastForward();
    if (DrawStepButton(width / 2.0f + 25, 0)) Step(Config()->stepsize);
    Scene* scene = GetActiveScene();
    if (scene) {
        PausePreRender();
        BeginTextureMode(g_viewport_target);
        ClearBackground(BLANK);
        for (size_t i = 0; i < scene->worlds.size; i++) {
            if (scene->worlds.data[i]->draw)
                scene->worlds.data[i]->draw(scene->worlds.data[i]);
            for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                if (scene->worlds.data[i]->systems.data[j]->draw)
                    scene->worlds.data[i]->systems.data[j]->draw(scene->worlds.data[i]->systems.data[j]);
        }
        EndTextureMode();
        ResumePreRender();
    }
    DrawTexturePro(
        g_viewport_target.texture,
        (Rectangle){ 0, g_viewport_target.texture.height - height + 26, width, -1*((int)height - 26) },
        (Rectangle){ 0, 26, width, height - 26 },
        (Vector2){ 0, 0 },
        0.0f,
        (Color){ 255, 255, 255, 255 });
    if (g_show_hints && HoveredPanel() && strcmp(HoveredPanel(), "Viewport") == 0) {
        DrawCurrentBinds(0, 26);
    }
}

static void PanCamera() {
    if (g_rfocused && !ViewCameraLocked()) {
        Vector2 delta = GetMouseDelta();
        Config()->camera.target = Vector2Add(Vector2Scale(delta, -1.0f / Config()->camera.zoom), Config()->camera.target);
    }
}

static void ZoomCamera() {
    if (g_zfocused && !ViewCameraLocked()) {
        Vector2 delta = GetMouseDelta();
        Vector2 focus = Vector2Add(GetViewportPosition(), Vector2Scale(GetViewportSlice(), 0.5f));
        Vector2 prev = Vector2Subtract(Vector2Subtract(GetMousePosition(), delta), focus);
        Vector2 current = Vector2Subtract(GetMousePosition(), focus);
        Config()->camera.zoom += Vector2Length(delta) * 0.01f * (Vector2Length(prev) > Vector2Length(current) ? -1.0f : 1.0f);
        if (Config()->camera.zoom < 1e-6f) Config()->camera.zoom = 1e-6f;
    }
}

static void ResetCamera() {
    Config()->camera = (Camera2D){ (Vector2){ 0, 0 }, (Vector2){ 0, 0 }, 0, 1.0f };
}

static void UpdateViewportPanel(float width, float height) {
    ResizeViewportTarget();
    const char* hpanel = HoveredPanel();
    BOOL hovered = hpanel && strcmp(hpanel, "Viewport") == 0;
    if (InputButtonReleased(IK_MOUSERIGHT)) g_rfocused = FALSE;
    if (InputKeyReleased(IK_ZOOM)) g_zfocused = FALSE;
    if (InputButtonPressed(IK_MOUSERIGHT) && hovered) g_rfocused = TRUE;
    if (InputKeyPressed(IK_ZOOM) && hovered) g_zfocused = TRUE;
    if (!ViewCameraLocked() && hovered) {
        Config()->camera.zoom += GetMouseWheelMove() * 0.1f;
        if (Config()->camera.zoom < 1e-6f) Config()->camera.zoom = 1e-6f;
    }
}

static void CleanViewportPanel() {
    UnloadRenderTexture(g_viewport_target);
}

Panel GenerateViewportPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Viewport");
	p.draw = DrawViewportPanel;
    p.update = UpdateViewportPanel;
    p.clean = CleanViewportPanel;
	p.flush = TRUE;
    g_viewport_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    AddBind("pan viewport camera", PanCamera, (BindCommand){ IK_MOUSERIGHT, BIND_BUTTON_END });
    AddBind("zoom viewport camera", ZoomCamera, (BindCommand){ IK_ZOOM, BIND_KEY_END });
    AddBind("toggle input hints", ToggleHints, (BindCommand){ IK_TOGGLE_HINTS, BIND_KEY_PRESSED });
    AddBind("reset viewport camera", ResetCamera, (BindCommand){ IK_RESET_CAMERA, BIND_KEY_PRESSED });
	return p;
}

Vector2 GetViewportSlice() {
    return g_viewport_slice;
}

Vector2 GetViewportPosition() {
    return g_viewport_position;
}

void SetViewportSlice(Vector2 slice) {
    g_viewport_slice = slice;
}

void SetViewportPosition(Vector2 pos) {
    g_viewport_position = pos;
}
