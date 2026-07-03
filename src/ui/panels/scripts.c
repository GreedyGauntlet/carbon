#include "scripts.h"
#include "core/application.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ui/ui.h"

static size_t g_selected = (size_t)-1;

static void DrawScriptsPanel(float width, float height) {
    Scene* scene = GetActiveScene();
    if (!scene || scene->scripts.names.size == 0) {
        UISetCursor(width / 2.0f - (UITextWidth("No Scripts Loaded") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Scripts Loaded");
        return;
    }
    UIDrawText("Loaded Scripts");
    UIDivider(width - 20);
    for (size_t i = 0; i < scene->scripts.names.size; i++) {
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
            DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
            if (InputButtonPressed(IK_MOUSELEFT)) g_selected = g_selected == i ? (size_t)-1 : i;
        }
        if (g_selected == i) DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
        UIDrawText("%d. %s", (int)i + 1, scene->scripts.names.data[i]);
        if (g_selected == i) {
            DrawRectangle(UIGetCursor().x + 5, UIGetCursor().y + 3, 1, 8, MappedColor(UI_TEXT_COLOR));
            DrawRectangle(UIGetCursor().x + 5, UIGetCursor().y + 11, 8, 1, MappedColor(UI_TEXT_COLOR));
            DrawTriangle(
                (Vector2){ UIGetCursor().x + 13, UIGetCursor().y + 7 },
                (Vector2){ UIGetCursor().x + 13, UIGetCursor().y + 16 },
                (Vector2){ UIGetCursor().x + 18, UIGetCursor().y + 11 },
                MappedColor(UI_TEXT_COLOR));
            UIMoveCursor(23, 0);
            UIDrawTextWrapped(width - 33, "%s", scene->scripts.descriptions.data[i]);
        }
    }
 }

Panel GenerateScriptsPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Scripts");
	p.draw = DrawScriptsPanel;
	return p;
}
