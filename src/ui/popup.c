#include "popup.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "core/config.h"
#include "ui/ui.h"
#include <float.h>

static float g_ff_speed = 1.0f;
static size_t g_step_count = 0;

static int edit_viewport_config(size_t x, size_t y, size_t w, size_t h) {
    float width = 365;
    float height = 250;
    float xpos = x + ((w - width) / 2.0f);
    float ypos = y + ((h - height) / 2.0f);
    float button_width = 200;
    UISetPosition(0, 0);
    UISetCursor(0, ypos + 10);
    DrawRectangle(xpos, ypos, width, height, MappedColor(PANEL_BG_COLOR));
    UIMoveCursor(xpos + (width / 2) - (UITextWidth("Configure Viewport Toolbar") / 2), 0);
    UIDrawText("Configure Viewport Toolbar");

    UIMoveCursor(xpos, 25);
    UIDivider(width - 20);
    UIMoveCursor(xpos, 5);
    UIDrawText("Fast-Forward Speed");
    UIMoveCursor(xpos + ((width - 20) / 2.0f), -20);
    UIDragFloat(&g_ff_speed, 0.0f, FLT_MAX, 0.001f, (width - 20.0f)/2.0f);
    UIMoveCursor(xpos, 5);
    UIDrawText("Step Count");
    UIMoveCursor(xpos + ((width - 20) / 2.0f), -20);
    UIDragSize(&g_step_count, 1, 9999999999, 1, (width - 20.0f)/2.0f);
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

Popup* GenerateEmptyPopup() {
    return EZ_ALLOC(1, sizeof(Popup));
}

void CleanPopup(Popup* popup) {
    if (popup->options != 0)
        for (size_t i = 0; i < popup->options; i++)
            CleanPopup(((Popup**)popup->results)[i]);
    if (popup->options > 0) EZ_FREE(popup->results);
    EZ_FREE(popup);
}

Popup* GenerateViewportConfigPopup() {
    Popup* popup = GenerateEmptyPopup();
    popup->behavior = edit_viewport_config;
    g_ff_speed = Config()->ffspeed;
    g_step_count = Config()->stepsize;
    return popup;
}
