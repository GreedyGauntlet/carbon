#include "popup.h"
#include "core/application.h"
#include "core/config.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ui/ui.h"
#include <float.h>
#include <raymath.h>

static float g_ff_speed = 1.0f;
static size_t g_step_count = 0;
static int g_edit_ui_state = 0; // 0 = uninitialized, 1 = reflected config, 2 = editing config
static ARRLIST_UIConfig g_ui_config_state = { 0 };

static void poll_dividers_helper(UI* ui, size_t* index) {
    g_ui_config_state.data[*index].divide = ui->divide;
    if (ui->left) {
        *index += 1;
        poll_dividers_helper(GetLeftUI(ui), index);
    }
    if (ui->right) {
        *index += 1;
        poll_dividers_helper(GetRightUI(ui), index);
    }
    if (ui->panels.size > 0 ) *index += ui->panels.size - 1;
}

static void poll_dividers() {
    UI* root = GetPrimaryUI();
    size_t i = 0;
    poll_dividers_helper(root, &i);
}

static void draw_ui_config_helper(size_t x, size_t y, size_t w, size_t h, size_t* current, float wratio, float hratio) {
    static int s_held_current = -1;
    if (*current >= g_ui_config_state.size) return;
    UIConfig conf = g_ui_config_state.data[*current];
    if ((conf.left || conf.right) && !conf.vine) {
        float _x, _y, _d;
        if (conf.vertical) {
            float pct = conf.divide / (((float)h) * hratio);
            _d = (pct * h);
            _x = x;
            _y = y + _d - 1;
        } else {
            float pct = conf.divide / (((float)w) * wratio);
            _d = (pct * w);
            _x = x + _d - 1;
            _y = y;
        }
        BOOL mhovered = CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){_x - 1, _y - 1, (conf.vertical ? w : 2) + 2, (conf.vertical ? 2 : h) + 2});
        DrawRectangle(_x, _y, conf.vertical ? w : 2, conf.vertical ? 2 : h, mhovered || s_held_current == (int)(*current) ? MappedColor(PANEL_DIVIDER_HOVER_COLOR) : MappedColor(UI_DIVIDER_COLOR));
        if (mhovered && InputButtonDown(IK_MOUSELEFT)) {
            g_edit_ui_state = 2;
            s_held_current = *current;
        }
        if (InputButtonUp(IK_MOUSELEFT)) s_held_current = -1;
        if (s_held_current == (int)(*current))
            g_ui_config_state.data[*current].divide = conf.vertical ? ((_d + GetMouseDelta().y) / (float)h) *  (((float)h) * hratio) : ((_d + GetMouseDelta().x) / (float)w) *  (((float)w) * wratio);
        if (conf.left) {
            *current += 1;
            draw_ui_config_helper(x, y, conf.vertical ? w : _x - x, conf.vertical ? _y - y : h, current, wratio, hratio);
        }
        if (conf.right) {
            *current += 1;
            draw_ui_config_helper(_x, _y, conf.vertical ? w : w - (_x - x), conf.vertical ? h - (_y - y) : h, current, wratio, hratio);
        }
    }
    if (conf.vine) {
        *current += 1;
        draw_ui_config_helper(x, y, w, h, current, wratio, hratio);
    }
}

static int edit_editor_config_popup(size_t x, size_t y, size_t w, size_t h) {
    float width = 365;
    float height = 580;
    float xpos = x + ((w - width) / 2.0f);
    float ypos = y + ((h - height) / 2.0f);
    float button_width = 200;
    UISetPosition(0, 0);
    UISetCursor(0, ypos + 10);
    DrawRectangle(xpos, ypos, width, height, MappedColor(PANEL_BG_COLOR));
    UIMoveCursor(xpos + (width / 2) - (UITextWidth("Editor Settings") / 2), 0);
    UIDrawText("Editor Settings");

    UIMoveCursor(xpos, 25);
    UIDivider(width - 20);
    UIMoveCursor(xpos + (width / 2) - (UITextWidth("Viewport Toolbar") / 2) - 10, 5);
    UIDrawText("Viewport Toolbar");
    UIMoveCursor(xpos, 15);
    UIDrawText("Fast-Forward Speed");
    UIMoveCursor(xpos + ((width - 20) / 2.0f), -20);
    UIDragFloat(&g_ff_speed, 0.0f, FLT_MAX, 0.001f, (width - 20.0f)/2.0f);
    UIMoveCursor(xpos, 5);
    UIDrawText("Step Count");
    UIMoveCursor(xpos + ((width - 20) / 2.0f), -20);
    UIDragSize(&g_step_count, 1, 9999999999, 1, (width - 20.0f)/2.0f);
    UIMoveCursor(xpos, 5);
    UIDivider(width - 20);

    if (g_edit_ui_state == 0) {
        ARRLIST_UIConfig_clear(&g_ui_config_state);
        for (size_t i = 0; i < GetUIConfig()->size; i++)
            ARRLIST_UIConfig_add(&g_ui_config_state, GetUIConfig()->data[i]); 
        g_edit_ui_state = 1;
    }
    if (g_edit_ui_state == 1) poll_dividers();

    UIMoveCursor(xpos + (width / 2) - (UITextWidth("UI Layout") / 2) - 10, 5);
    UIDrawText("UI Layout");
    UIMoveCursor(xpos, 15);
    DrawRectangle(UIGetCursor().x, UIGetCursor().y, width - 20, 200, (Color){ 0, 0, 0, 90 });
    size_t ind = 0;
    draw_ui_config_helper(UIGetCursor().x, UIGetCursor().y, width - 20, 200, &ind, (float)GetScreenWidth() / (float)(width - 20), (float)GetScreenHeight() / 200.0f);
    UIMoveCursor(0, 210);
    if (UIButton("Reset", 80)) {
        ARRLIST_UIConfig_clear(&g_ui_config_state);
        g_edit_ui_state = 2;
    }
    UIMoveCursor(xpos + width - 10 - 160 - 20, -LINE_HEIGHT);
    if (g_edit_ui_state != 2) DisableUI();
    if (UIButton("Save", 80)) {
        // TODO:
        g_edit_ui_state = 1;
    }
    UIMoveCursor(xpos + width - 10 - 80 - 10, -LINE_HEIGHT);
    if (UIButton("Cancel", 80)) {
        ARRLIST_UIConfig_clear(&g_ui_config_state);
        g_edit_ui_state = 0;
    }
    EnableUI();
    UIMoveCursor(xpos, 5);
    UIDivider(width - 20);

    UISetCursor(xpos + (width / 2) - (button_width / 2), ypos + height - 70);
    if (UIButton("Confirm", button_width)) {
        Config()->ffspeed = g_ff_speed;
        Config()->stepsize = g_step_count;
        return 0;
    }
    UISetCursor(xpos + (width / 2) - (button_width / 2), ypos + height - 40);
    if (UIButton("Cancel", button_width)) return 0;
    return -1;
}

static void clean_editor_config_popup() {
    ARRLIST_UIConfig_clear(&g_ui_config_state);
    g_edit_ui_state = 0;
}

Popup* GenerateEmptyPopup() {
    return EZ_ALLOC(1, sizeof(Popup));
}

void CleanPopup(Popup* popup) {
    if (popup->clean) popup->clean();
    if (popup->options != 0)
        for (size_t i = 0; i < popup->options; i++)
            CleanPopup(((Popup**)popup->results)[i]);
    if (popup->options > 0) EZ_FREE(popup->results);
    EZ_FREE(popup);
}

Popup* GenerateEditorConfigPopup() {
    Popup* popup = GenerateEmptyPopup();
    popup->behavior = edit_editor_config_popup;
    popup->clean = clean_editor_config_popup;
    g_ff_speed = Config()->ffspeed;
    g_step_count = Config()->stepsize;
    return popup;
}
