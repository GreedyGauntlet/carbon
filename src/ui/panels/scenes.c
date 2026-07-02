#include "scenes.h"
#include "core/application.h"
#include "core/world.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "ui/panels/edit.h"
#include "ui/ui.h"
#include "util/logger.h"
#include <easyhash.h>

static uint64_t HashWorldPointer(World* ptr) { return ez_hash_uint64_t((uint64_t)ptr); }

DECLARE_HASHMAP(World*, BOOL, OpenedWorlds);
IMPL_HASHMAP(World*, BOOL, OpenedWorlds, HashWorldPointer);

static HASHMAP_OpenedWorlds g_openedworlds = { 0 };
static float g_scrolldiff = 0.0f;

static size_t DropdownSelectActiveScene(void* data, size_t index) {
    ARRLIST_StaticString* names = SceneNames();
    if (index != (size_t)-1) {
        SetScene(names->data[index]);
        SelectEntity((Entity){ 0, 0 });
        return index;
    } else {
        Scene* active = GetActiveScene();
        for (size_t i = 0; i < names->size; i++) {
            if (names->data[i] == active->name) return i;
        }
    }
    return 0;
}

static void DrawEntityOption(Entity e, float offset, float width, float height) {
    TagComponent* tc = GetComponent(e, TagComponent);
    UIMoveCursor(offset, 0);
    DrawRectangle(UIGetCursor().x - 33, UIGetCursor().y - 10, 1, 20, MappedColor(UI_TEXT_COLOR));
    DrawRectangle(UIGetCursor().x - 33, UIGetCursor().y + 10, 20, 1, MappedColor(UI_TEXT_COLOR));
    ARRLIST_EntityID* children = GetChildren(e);
    if (!children || children->size == 0) {
        DrawRectangle(UIGetCursor().x - 18, UIGetCursor().y + 10, 10, 1, MappedColor(UI_TEXT_COLOR));
    }
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 2 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width - UIGetCursor().x - 10, 20})) {
        DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) SelectEntity(e);
    }
    if (SelectedEntity().id == e.id && SelectedEntity().context == e.context)
        DrawRectangle(UIGetCursor().x - 2, UIGetCursor().y, width - UIGetCursor().x - 10, 20, MappedColor(UI_SELECTED_COLOR));
    UIDrawText("%s", tc->tag);
    if (children && children->size > 0 && CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x + offset - 22, UIGetCursor().y + UIGetPosition().y - 19, 19, 19})) {
        DrawRectangle(UIGetCursor().x + offset - 22, UIGetCursor().y - 19, 19, 19, MappedColor(UI_HIGHLIGHT_COLOR));
        if (InputButtonPressed(IK_MOUSELEFT)) tc->metacollapsed = !tc->metacollapsed;
    }
    if (children && children->size > 0 && !tc->metacollapsed) {
        DrawRectangle(UIGetCursor().x + offset - 17, UIGetCursor().y - 14, 9, 9, MappedColor(UI_TEXT_COLOR));
        DrawRectangle(UIGetCursor().x + offset - 16, UIGetCursor().y - 13, 7, 7, MappedColor(PANEL_BG_COLOR));
        for (size_t i = 0; i < children->size; i++) {
            Entity child = (Entity){ children->data[i], e.context };
            if (UIGetCursor().y < height) DrawEntityOption(child, offset + 20, width, height);
        }
    } else if (children && children->size > 0) {
        DrawRectangle(UIGetCursor().x + offset - 17, UIGetCursor().y - 14, 9, 9, MappedColor(UI_TEXT_COLOR));
    }
}

static int CountVisibleChildrenEntries(Entity e) {
    ARRLIST_EntityID* children = GetChildren(e);
    int count = 0;
    if (children && children->size > 0) {
        for (size_t i = 0; i < children->size; i++) {
            Entity child = (Entity){ children->data[i], e.context };
            count++;
            if (!GetComponent(child, TagComponent)->metacollapsed) count += CountVisibleChildrenEntries(child);
        }
    }
    return count;
}

static int CountVisibleEntityEntries(World* world) {
    ARRLIST_EntityID* entities = GetEntities(world, TagComponent);
    int count = 0;
    if (entities) {
        for (size_t i = 0; i < entities->size; i++) {
            Entity e = (Entity){ entities->data[i], world };
            if (HasParent(e)) continue;
            count++;
            TagComponent* tc = GetComponent(e, TagComponent);
            if (!tc->metacollapsed) count += CountVisibleChildrenEntries(e);
        }
    }
    return count;
}

static int CountVisibleEntries() {
    Scene* scene = GetActiveScene();
    int count = 0;
    for (size_t i = 0; i < scene->worlds.size; i++) {
        if (!HASHMAP_OpenedWorlds_has(&g_openedworlds, scene->worlds.data[i])) HASHMAP_OpenedWorlds_set(&g_openedworlds, scene->worlds.data[i], TRUE);
        count += 1 + (HASHMAP_OpenedWorlds_get(&g_openedworlds, scene->worlds.data[i]) ? CountVisibleEntityEntries(scene->worlds.data[i]) : 0 );
    }
    return count;
}

static void DrawScenesPanel(float width, float height) {
    Scene* scene = GetActiveScene();
    if (!scene) {
        UISetCursor(width / 2.0f - (UITextWidth("No Active Scene") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Active Scene");
        return;
    }
    if (HoveredPanel() && strcmp(HoveredPanel(), "Scenes") == 0) {
        if (InputKeyPressed(IK_ENTER)) g_scrolldiff = 0.0f;
        g_scrolldiff -= 18.0f * GetMouseWheelMove();
    }
    int vbe = CountVisibleEntries();
    if (g_scrolldiff > vbe * LINE_HEIGHT - height + 85) g_scrolldiff = vbe * LINE_HEIGHT - height + 85;
    if (g_scrolldiff < 0) g_scrolldiff = 0.0f;
    UIMoveCursor(0, 45 - g_scrolldiff);
    for (size_t i = 0; i < scene->worlds.size; i++) {
        if (!HASHMAP_OpenedWorlds_has(&g_openedworlds, scene->worlds.data[i])) HASHMAP_OpenedWorlds_set(&g_openedworlds, scene->worlds.data[i], TRUE);
        ARRLIST_EntityID* entities = GetEntities(scene->worlds.data[i], TagComponent);
        BOOL opened = HASHMAP_OpenedWorlds_get(&g_openedworlds, scene->worlds.data[i]);
        UIMoveCursor(20.0f, 0);
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){UIGetCursor().x - 30 + UIGetPosition().x, UIGetCursor().y + UIGetPosition().y, width, LINE_HEIGHT})) {
            DrawRectangle(UIGetCursor().x - 30, UIGetCursor().y, width, 20, MappedColor(UI_HIGHLIGHT_COLOR));
            if (InputButtonPressed(IK_MOUSELEFT)) HASHMAP_OpenedWorlds_set(&g_openedworlds, scene->worlds.data[i], !opened);
        }
        DrawCircle(UIGetCursor().x - 12, UIGetCursor().y + 10, 6, MappedColor(UI_TEXT_COLOR));
        if (opened) DrawCircle(UIGetCursor().x - 12, UIGetCursor().y + 10, 5, MappedColor(PANEL_BG_COLOR));
        UIDrawText("%s", scene->worlds.data[i]->name);
        if (entities && opened) {
            for (size_t j = 0; j < entities->size; j++) {
                Entity e = (Entity){ entities->data[j], scene->worlds.data[i] };
                if (!HasParent(e) && UIGetCursor().y < height) DrawEntityOption(e, 40.0f, width, height);
            }
        }
    }
    UISetCursor(10, 10);
    DrawRectangle(0, 0, width, 45, MappedColor(PANEL_BG_COLOR));
    UIDrawText("Active Scene");
    UIMoveCursor(UITextWidth("Active Scene") + 5, -LINE_HEIGHT);
    UIDropdownMenu(width - UITextWidth("Active Scene") - 25, SceneNames()->size, (char**)SceneNames()->data, DropdownSelectActiveScene, NULL);
    UIDivider(width - 20);
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
