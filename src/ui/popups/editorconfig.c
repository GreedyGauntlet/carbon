#include "editorconfig.h"
#include "core/application.h"
#include "core/config.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "data/fonts.h"
#include "ui/popup.h"
#include <float.h>
#include <raymath.h>

static float g_ff_speed = 1.0f;
static size_t g_step_count = 0;
static int g_edit_ui_state = 0; // 0 = uninitialized, 1 = reflected config, 2 = editing config
static ARRLIST_UIConfig g_ui_config_state = { 0 };
static BOOL g_prompt_ui_add = FALSE;
static ARRLIST_DynamicString g_ui_config_options = { 0 };

static BOOL invalid_ui_config() {
    for (size_t i = 0; i < g_ui_config_state.size; i++) {
        if (!g_ui_config_state.data[i].left &&
            !g_ui_config_state.data[i].right &&
            strlen(g_ui_config_state.data[i].name) == 0) 
            return TRUE;
    }
    return FALSE;
}

static Rectangle ui_bounding_box_helper(size_t index, size_t* curr, Rectangle box, BOOL* ret) {
    size_t current = *curr;
    if (*curr == index) {
        *ret = TRUE;
        return box;
    } else if (!g_ui_config_state.data[current].left && !g_ui_config_state.data[current].right) {
        *ret = FALSE;
        return box;
    }
    *curr += 1;
    Rectangle r = ui_bounding_box_helper(index, curr, (Rectangle){
        box.x, box.y,
        g_ui_config_state.data[current].vertical ? box.width : g_ui_config_state.data[current].divide,
        g_ui_config_state.data[current].vertical ? g_ui_config_state.data[current].divide : box.height
    }, ret);
    if (*ret) return r;
    *curr += 1;
    r = ui_bounding_box_helper(index, curr, (Rectangle){
        g_ui_config_state.data[current].vertical ? box.x : box.x + g_ui_config_state.data[current].divide,
        g_ui_config_state.data[current].vertical ? box.y + g_ui_config_state.data[current].divide : box.y,
        g_ui_config_state.data[current].vertical ? box.width : box.width - g_ui_config_state.data[current].divide,
        g_ui_config_state.data[current].vertical ? box.height - g_ui_config_state.data[current].divide : box.height
    }, ret);
    if (*ret) return r;
    *ret = FALSE;
    return box;
}

static Rectangle ui_bounding_box(size_t index) {
    size_t i = 0;
    BOOL ret = FALSE;
    return ui_bounding_box_helper(index, &i, (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight()}, &ret);
}

static size_t edit_ui_dropdown(void* data, size_t index, BOOL cancel) {
    if (index != (size_t)-1 || cancel) g_prompt_ui_add = FALSE;
    if (index != (size_t)-1 && index < g_ui_config_options.size - 1) {
        size_t cind = (size_t)data;
        UIConfig* conf = &(g_ui_config_state.data[cind]);
        size_t starting = strlen(conf->name) > 0 ? 0 : 2;
        if (index < starting) {
            Rectangle bounds = ui_bounding_box(cind);
            conf->left = TRUE;
            conf->right = TRUE;
            if (index == 0) { // vertical
                conf->divide = bounds.height / 2.0f;
                conf->vertical = TRUE;
            } else { // horizontal
                conf->divide = bounds.width / 2.0f;
                conf->vertical = FALSE;
            }
            ARRLIST_UIConfig_insert(&g_ui_config_state, (UIConfig){ 0 }, cind + 1);
            ARRLIST_UIConfig_insert(&g_ui_config_state, (UIConfig){ 0 }, cind + 1);
        } else {
            while(conf->vine) {
                cind++;
                conf = &(g_ui_config_state.data[cind]);
            }
            if (strlen(conf->name) == 0) {
                strcpy(conf->name, g_ui_config_options.data[index]);
                conf->vine = FALSE;
            } else {
                conf->vine = TRUE;
                UIConfig newconf = { 0 };
                strcpy(newconf.name, g_ui_config_options.data[index]);
                ARRLIST_UIConfig_insert(&g_ui_config_state, newconf, cind + 1);
            }
        }
    }
    return -1;
}

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

static void draw_ui_config_helper(size_t x, size_t y, size_t w, size_t h, size_t* current, float wratio, float hratio, int vinesize) {
    static int s_held_current = -1;
    size_t bak_current = *current;
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
        BOOL mhovered = !g_prompt_ui_add && CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){_x - 1, _y - 1, (conf.vertical ? w : 2) + 2, (conf.vertical ? 2 : h) + 2});
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
            draw_ui_config_helper(x, y, conf.vertical ? w : _x - x, conf.vertical ? _y - y : h, current, wratio, hratio, vinesize);
        }
        if (conf.right) {
            *current += 1;
            draw_ui_config_helper(_x, _y, conf.vertical ? w : w - (_x - x), conf.vertical ? h - (_y - y) : h, current, wratio, hratio, vinesize);
        }
    }
    if (conf.vine) {
        if (vinesize == 0) {
            vinesize++;
            size_t cc = bak_current;
            while(g_ui_config_state.data[cc].vine) { cc++; vinesize++; }
        }
        *current += 1;
        draw_ui_config_helper(x, y, w, h, current, wratio, hratio, vinesize);
    }
    if (!conf.left && !conf.right) {
        BOOL mhovered = !g_prompt_ui_add && !conf.vine && s_held_current == -1 && CheckCollisionPointRec(Vector2Subtract(GetMousePosition(), UIGetPosition()), (Rectangle){x + 4, y + 4, w - 8, h - 8});
        if (mhovered) DrawRectangle(x, y, w + 1, h + 1, (Color){ 255, 255, 255, 110 });
        if (mhovered && InputButtonPressed(IK_MOUSELEFT)) {
            g_prompt_ui_add = TRUE;
            // is this eaiser and more efficient than using malloc? yes! is this unsafe? also yes!
            ARRLIST_DynamicString_clear(&g_ui_config_options);
            if (strlen(conf.name) == 0) {
                ARRLIST_DynamicString_add(&g_ui_config_options, "Split vertical");
                ARRLIST_DynamicString_add(&g_ui_config_options, "Split horizontal");
            }
            for (size_t i = 0; i < EditorPanels()->size; i++) {
                BOOL in_use = FALSE;
                for (size_t j = 0; j < g_ui_config_state.size; j++) {
                    if (strcmp(g_ui_config_state.data[j].name, EditorPanels()->data[i].name) == 0) {
                        in_use = TRUE;
                        break;
                    }
                }
                if (!in_use) ARRLIST_DynamicString_add(&g_ui_config_options, EditorPanels()->data[i].name);
            }
            ARRLIST_DynamicString_add(&g_ui_config_options, "Cancel");
            UIFloatingDropdown(150, GetMousePosition(), g_ui_config_options.size, g_ui_config_options.data, edit_ui_dropdown, (void*)bak_current);
        }
        size_t cc = bak_current;
        while (g_ui_config_state.data[cc].vine) { cc++; }
        size_t linecount = (cc - bak_current);
        float override_size = 18.0f;
        Vector2 tsize = MeasureTextEx(FontAsset(), conf.name, override_size, 0);
        DrawTextEx(
            FontAsset(),
            conf.name,
            (Vector2){
                x + (w/2.0f) - (tsize.x/2.0f),
                y + (h/2.f) - (tsize.y/2.0f) + (vinesize > 0 ? ((float)vinesize * tsize.y / -2.0f) + (linecount * tsize.y) + (tsize.y / 2.0f): 0)},
            override_size,
            0,
            MappedColor(UI_TEXT_COLOR));
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
    draw_ui_config_helper(UIGetCursor().x, UIGetCursor().y, width - 20, 200, &ind, (float)GetScreenWidth() / (float)(width - 20), (float)GetScreenHeight() / 200.0f, 0);
    UIMoveCursor(0, 210);
    if (UIButton("Reset", 80)) {
        ARRLIST_UIConfig_clear(&g_ui_config_state);
        ARRLIST_UIConfig_add(&g_ui_config_state, (UIConfig){ 0 });
        g_edit_ui_state = 2;
    }
    UIMoveCursor(xpos + width - 10 - 160 - 20, -LINE_HEIGHT);
    if (g_edit_ui_state != 2 || invalid_ui_config()) DisableUI();
    if (UIButton("Save", 80)) {
        SetUIConfig(&g_ui_config_state);
        g_edit_ui_state = 0;
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
    if (UIButton("Confirm", button_width) && !g_prompt_ui_add) {
        ConfigSetFloat("ffspeed", g_ff_speed);
        ConfigSetSize("stepsize", g_step_count);
        return 0;
    }
    UISetCursor(xpos + (width / 2) - (button_width / 2), ypos + height - 40);
    if (UIButton("Cancel", button_width) && !g_prompt_ui_add) return 0;
    return -1;
}

static void clean_editor_config_popup() {
    ARRLIST_UIConfig_clear(&g_ui_config_state);
    ARRLIST_DynamicString_clear(&g_ui_config_options);
    g_edit_ui_state = 0;
}

Popup* GenerateEditorConfigPopup() {
    Popup* popup = GenerateEmptyPopup();
    popup->behavior = edit_editor_config_popup;
    popup->clean = clean_editor_config_popup;
    g_ff_speed = ConfigGetFloat("ffspeed");
    g_step_count = ConfigGetSize("stepsize");
    return popup;
}
