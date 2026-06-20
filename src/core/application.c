#include "application.h"
#include "data/definitions.h"
#include "core/scene.h"

static Application g_application = { 0 };
static size_t g_resolution_width = 1600;
static size_t g_resolution_height = 900;

void InitializeApplication() {
    #ifndef PROD_BUILD
    g_application.memory = EZ_ALLOCATED();
    #endif
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(g_resolution_width, g_resolution_height, g_application.name == NULL ? "Carbon Engine" : g_application.name);
}

void SetApplicationName(const char* name) {
    g_application.name = name;
}

void SetApplicationGoodbye(const char* goodbye) {
    g_application.goodbye = goodbye;
}

void SetApplicationSize(const size_t width, const size_t height) {
    g_resolution_width = width;
    g_resolution_height = height;
}

void RunApplication() {
    while(!WindowShouldClose()) {
        if (g_application.scenes.size > 0) {
            Scene* scene = g_application.scenes.data[g_application.current];
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                int mb = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_RIGHT;
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousebutton)
                        scene->worlds.data[i]->mousebutton(scene->worlds.data[i], mb, INPUTPRESS);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousebutton)
                            scene->worlds.data[i]->systems.data[j]->mousebutton(scene->worlds.data[i]->systems.data[j], mb, INPUTPRESS);
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                int mb = IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_RIGHT;
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousebutton)
                        scene->worlds.data[i]->mousebutton(scene->worlds.data[i], mb, INPUTRELEASE);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousebutton)
                            scene->worlds.data[i]->systems.data[j]->mousebutton(scene->worlds.data[i]->systems.data[j], mb, INPUTRELEASE);
                }
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                int mb = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_RIGHT;
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousebutton)
                        scene->worlds.data[i]->mousebutton(scene->worlds.data[i], mb, INPUTDOWN);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousebutton)
                            scene->worlds.data[i]->systems.data[j]->mousebutton(scene->worlds.data[i]->systems.data[j], mb, INPUTDOWN);
                }
            }
            Vector2 mdelt = GetMouseDelta();
            Vector2 mscroll = GetMouseWheelMoveV();
            if (mdelt.x != 0 || mdelt.y != 0) {
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousemove)
                        scene->worlds.data[i]->mousemove(scene->worlds.data[i], GetMousePosition());
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousemove)
                            scene->worlds.data[i]->systems.data[j]->mousemove(scene->worlds.data[i]->systems.data[j], GetMousePosition());
                }
            }
            if (mscroll.x != 0 || mscroll.y != 0) {
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousescroll)
                        scene->worlds.data[i]->mousescroll(scene->worlds.data[i], mscroll);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousescroll)
                            scene->worlds.data[i]->systems.data[j]->mousescroll(scene->worlds.data[i]->systems.data[j], mscroll);
                }
            }
            int kcode = 0;
            while ((kcode = GetKeyPressed()) != 0) {
                if (!HASHMAP_KeyMap_has(&g_application.keymap, kcode))
                    ARRLIST_int_add(&g_application.keylist, kcode);
                HASHMAP_KeyMap_set(&g_application.keymap, kcode, TRUE);
            }
            for (size_t i = 0; i < g_application.keylist.size; i++) {
                if (!IsKeyDown(g_application.keylist.data[i]))
                    HASHMAP_KeyMap_set(&g_application.keymap, g_application.keylist.data[i], FALSE);
                if (HASHMAP_KeyMap_get(&g_application.keymap, g_application.keylist.data[i])) {
                    for (size_t j = 0; j < scene->worlds.size; j++) {
                        if (scene->worlds.data[j]->key) {
                            scene->worlds.data[j]->key(scene->worlds.data[j], g_application.keylist.data[i], INPUTDOWN);
                            if (IsKeyPressed(g_application.keylist.data[i]))
                                scene->worlds.data[j]->key(scene->worlds.data[j], g_application.keylist.data[i], INPUTPRESS);
                        }
                        for (size_t k = 0; k < scene->worlds.data[j]->systems.size; k++)
                            if (scene->worlds.data[j]->systems.data[k]->key) {
                                scene->worlds.data[j]->systems.data[k]->key(scene->worlds.data[j]->systems.data[k], g_application.keylist.data[j], INPUTDOWN);
                                if (IsKeyPressed(g_application.keylist.data[i]))
                                    scene->worlds.data[j]->systems.data[k]->key(scene->worlds.data[j]->systems.data[k], g_application.keylist.data[j], INPUTPRESS);
                            }
                    }
                } else if (IsKeyReleased(g_application.keylist.data[i])) {
                    for (size_t j = 0; j < scene->worlds.size; j++) {
                        if (scene->worlds.data[j]->key) {
                            scene->worlds.data[j]->key(scene->worlds.data[j], g_application.keylist.data[i], INPUTRELEASE);
                        }
                        for (size_t k = 0; k < scene->worlds.data[j]->systems.size; k++)
                            if (scene->worlds.data[j]->systems.data[k]->key)
                                scene->worlds.data[j]->systems.data[k]->key(scene->worlds.data[j]->systems.data[k], g_application.keylist.data[j], INPUTRELEASE);
                    }
                }
            }
            for (size_t i = 0; i < scene->worlds.size; i++) {
                if (scene->worlds.data[i]->update)
                    scene->worlds.data[i]->update(scene->worlds.data[i], GetFrameTime());
                for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                    if (scene->worlds.data[i]->systems.data[j]->update)
                        scene->worlds.data[i]->systems.data[j]->update(scene->worlds.data[i]->systems.data[j], GetFrameTime());
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
}

void DestroyApplication() {
    CloseWindow();
    HASHMAP_KeyMap_clear(&g_application.keymap);
    ARRLIST_int_clear(&g_application.keylist);
    for (size_t i = 0; i < g_application.scenes.size; i++) DestroyScene(g_application.scenes.data[i]);
    ARRLIST_ScenePtr_clear(&g_application.scenes);
    #ifndef PROD_BUILD
    EZ_ASSERT(EZ_ALLOCATED() == 0, "Memory cleanup revealed a leak of %d bytes", (int)(EZ_ALLOCATED() - g_application.memory));
    #endif
    EZ_INFO("%s", g_application.goodbye == NULL ? "Hoc est omnia, amice." : g_application.goodbye);
}

void AddScene(Scene* scene) {
    ARRLIST_ScenePtr_add(&g_application.scenes, scene);
}

void SetScene(const char* name) {
    for (size_t i = 0; i < g_application.scenes.size; i++) {
        if (strcmp(g_application.scenes.data[i]->name, name) == 0) {
            g_application.current = i;
            return;
        }
    }
    EZ_ASSERT(FALSE, "Unable to set the scene \"%s\" that does not exist", name);
}
