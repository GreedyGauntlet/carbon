#include "pick.h"
#include "ecs/entity.h"

static RenderTexture2D g_pick_target = { 0 };
static ARRLIST_Entity g_pick_entries = { 0 };
static BOOL g_picking_mode = FALSE;

void ResizePickTarget() {
    if (g_pick_target.texture.width != GetScreenWidth() || g_pick_target.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(g_pick_target);
        g_pick_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }
}

void BeginPickPass() {
    ARRLIST_Entity_wipe(&g_pick_entries);
    BeginTextureMode(g_pick_target);
    ClearBackground(BLANK);
    g_picking_mode = TRUE;
}

void EndPickPass() {
    EndTextureMode();
    g_picking_mode = FALSE;
}

BOOL IsPicking() {
    return g_picking_mode;
}

size_t RegisterPickable(Entity e) {
    size_t index = g_pick_entries.size;
    ARRLIST_Entity_add(&g_pick_entries, (Entity){ e.id, e.context });
    return index;
}

Color PickColorFor(size_t index) {
    return (Color){ index & 0xFF, (index >> 8) & 0xFF, (index >> 16) & 0xFF, 255 };
}

Entity ResolvePick(Vector2 localPos) {
    if (localPos.x < 0 || localPos.y < 0 ||
        localPos.x >= g_pick_target.texture.width ||
        localPos.y >= g_pick_target.texture.height) return (Entity){ 0, 0 };
    Image img = LoadImageFromTexture(g_pick_target.texture);
    int flippedY = img.height - 1 - (int)localPos.y;
    Color c = GetImageColor(img, (int)localPos.x, flippedY);
    UnloadImage(img);
    if (c.a == 0) return (Entity){ 0, 0 };
    size_t index = (size_t)c.r | ((size_t)c.g << 8) | ((size_t)c.b << 16);
    if (index >= g_pick_entries.size) return (Entity){ 0, 0 };
    return g_pick_entries.data[index];
}

void CleanPicking() {
    UnloadRenderTexture(g_pick_target);
    ARRLIST_Entity_clear(&g_pick_entries);
}
