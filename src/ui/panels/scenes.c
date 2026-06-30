#include "scenes.h"
#include "core/application.h"
#include "core/world.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "ui/ui.h"
#include <easyhash.h>

static uint64_t HashWorldPointer(World* ptr) { return ez_hash_uint64_t((uint64_t)ptr); }

DECLARE_HASHMAP(World*, BOOL, OpenedWorlds);
IMPL_HASHMAP(World*, BOOL, OpenedWorlds, HashWorldPointer);

static HASHMAP_OpenedWorlds g_openedworlds = { 0 };

static void DrawEntityOption(Entity e, float offset) {
    TagComponent* tc = GetComponent(e, TagComponent);
    UIMoveCursor(offset, 0);
    DrawRectangle(UIGetCursor().x - 33, UIGetCursor().y - 10, 1, 20, MappedColor(UI_TEXT_COLOR));
    DrawRectangle(UIGetCursor().x - 33, UIGetCursor().y + 10, 20, 1, MappedColor(UI_TEXT_COLOR));
    ARRLIST_EntityID* children = GetChildren(e);
    if (!children || children->size == 0) {
        DrawRectangle(UIGetCursor().x - 18, UIGetCursor().y + 10, 10, 1, MappedColor(UI_TEXT_COLOR));
    }
    UIDrawText("%s", tc->tag);
    if (children && children->size > 0) {
        DrawRectangle(UIGetCursor().x + offset - 17, UIGetCursor().y - 14, 9, 9, MappedColor(UI_TEXT_COLOR));
        DrawRectangle(UIGetCursor().x + offset - 16, UIGetCursor().y - 13, 7, 7, MappedColor(PANEL_BG_COLOR));
        for (size_t i = 0; i < children->size; i++) {
            Entity child = (Entity){ children->data[i], e.context };
            DrawEntityOption(child, offset + 20);
        }
    }
}

static void DrawScenesPanel(float width, float height) {
    Scene* scene = GetActiveScene();
    if (!scene) {
        UISetCursor(width / 2.0f - (UITextWidth("No Active Scene") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Active Scene");
        return;
    }
    UIDrawText("TODO");
    UIDivider(width - 20);
    for (size_t i = 0; i < scene->worlds.size; i++) {
        if (!HASHMAP_OpenedWorlds_has(&g_openedworlds, scene->worlds.data[i])) HASHMAP_OpenedWorlds_set(&g_openedworlds, scene->worlds.data[i], TRUE);
        ARRLIST_EntityID* entities = GetEntities(scene->worlds.data[i], TagComponent);
        BOOL opened = HASHMAP_OpenedWorlds_get(&g_openedworlds, scene->worlds.data[i]);
        UIMoveCursor(20.0f, 0);
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 30 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width, 20})) {
            DrawRectangle(UIGetCursor().x - 30, UIGetCursor().y, width, 20, MappedColor(UI_HIGHLIGHT_COLOR));
            if (InputButtonPressed(IK_MOUSELEFT)) HASHMAP_OpenedWorlds_set(&g_openedworlds, scene->worlds.data[i], !opened);
        }
        DrawCircle(UIGetCursor().x - 12, UIGetCursor().y + 10, 6, MappedColor(UI_TEXT_COLOR));
        if (opened) DrawCircle(UIGetCursor().x - 12, UIGetCursor().y + 10, 5, MappedColor(PANEL_BG_COLOR));
        UIDrawText("%s", scene->worlds.data[i]->name);
        if (entities && opened) {
            for (size_t j = 0; j < entities->size; j++) {
                Entity e = (Entity){ entities->data[j], scene->worlds.data[i] };
                if (!HasParent(e)) DrawEntityOption(e, 40.0f);
            }
        }
    }
}

static void CleanScenesPanel() {
    HASHMAP_OpenedWorlds_clear(&g_openedworlds);
}

Panel GenerateScenesPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Scenes");
	p.draw = DrawScenesPanel;
    p.clean = CleanScenesPanel;
	return p;
}
