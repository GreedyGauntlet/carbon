#include "application.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "data/fonts.h"
#include "core/scene.h"
#include "core/world.h"
#include "core/config.h"
#include "ui/panels/viewport.h"
#include "ui/panels/console.h"
#include "ui/panels/scenes.h"
#include "ui/panels/graph.h"
#include "ui/panels/edit.h"
#include "ecs/components.h"
#include "ecs/entity.h"

static Application g_application = { 0 };
static size_t g_resolution_width = 1600;
static size_t g_resolution_height = 900;
static Vector2 g_windowsize = { -1.0f, -1.0f };
static BOOL g_playing = FALSE;
static BOOL g_fastforward = FALSE;
static size_t g_steps = 0;

static void ApplicationResized() {
    if ((g_windowsize.x == -1.0f && g_windowsize.y == -1.0f) ||
        (g_windowsize.x != GetScreenWidth() || g_windowsize.y != GetScreenHeight())) {
        g_windowsize.x = GetScreenWidth();
        g_windowsize.y = GetScreenHeight();
        ResizeUI(g_application.ui);
    }
}

void InitializeApplication() {
    #ifndef PROD_BUILD
    g_application.memory = EZ_ALLOCATED();
    #endif
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(g_resolution_width, g_resolution_height, g_application.name == NULL ? "Carbon Engine" : g_application.name);
    InitializeInput();
    InitializeColors();
    InitializeFonts();
    g_application.ui = GenerateUI();
    g_application.ui->left = GenerateUI();
    g_application.ui->right = GenerateUI();
    ((UI*)g_application.ui->right)->right = GenerateUI();
    ((UI*)g_application.ui->right)->left = GenerateUI();
    ((UI*)g_application.ui->right)->divide = GetScreenHeight() - 560;
    ((UI*)g_application.ui->right)->vertical = TRUE;
    ((UI*)g_application.ui->left)->right = GenerateUI();
    ((UI*)g_application.ui->left)->left = GenerateUI();
    ((UI*)((UI*)g_application.ui->left)->left)->left = GenerateUI();
    ((UI*)((UI*)g_application.ui->left)->left)->right = GenerateUI();
    ((UI*)((UI*)g_application.ui->left)->left)->divide = GetScreenHeight() - 420;
    ((UI*)((UI*)g_application.ui->left)->left)->vertical = TRUE;
    ((UI*)g_application.ui->left)->divide = 350;
    ARRLIST_Panel_add(&(((UI*)(((UI*)g_application.ui->right)->right))->panels), GenerateEditPanel());
    ARRLIST_Panel_add(&(((UI*)(((UI*)g_application.ui->right)->left))->panels), GenerateConsolePanel());
    ARRLIST_Panel_add(&(((UI*)(((UI*)g_application.ui->left)->right))->panels), GenerateViewportPanel());
    ARRLIST_Panel_add(&(GetLeftUI(GetLeftUI(GetLeftUI(g_application.ui)))->panels), GenerateScenesPanel());
    ARRLIST_Panel_add(&(GetRightUI(GetLeftUI(GetLeftUI(g_application.ui)))->panels), GenerateGraphPanel());
    g_application.ui->divide = 1250;
    SetPrimaryUI(g_application.ui);
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
        ApplicationResized();
        UpdateUI(g_application.ui);
        if (g_playing && g_steps > 0) {
            g_steps--;
            if (g_steps == 0) Pause();
        }
        if (g_application.scenes.size > 0 && g_playing) {
            Scene* scene = g_application.scenes.data[g_application.current];
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                int mb = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_RIGHT;
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousebutton)
                        scene->worlds.data[i]->mousebutton(scene->worlds.data[i], mb, INPUTPRESS);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousebutton)
                            scene->worlds.data[i]->systems.data[j]->mousebutton(scene->worlds.data[i]->systems.data[j], mb, INPUTPRESS);
                    ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                    if (scripts) {
                        for (size_t j = 0; j < scripts->size; j++) {
                            Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                            ScriptComponent* sc = GetComponent(e, ScriptComponent);
                            if (sc->mousebutton) sc->mousebutton(e, mb, INPUTPRESS);
                        }
                    }
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
                    ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                    if (scripts) {
                        for (size_t j = 0; j < scripts->size; j++) {
                            Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                            ScriptComponent* sc = GetComponent(e, ScriptComponent);
                            if (sc->mousebutton) sc->mousebutton(e, mb, INPUTRELEASE);
                        }
                    }
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
                    ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                    if (scripts) {
                        for (size_t j = 0; j < scripts->size; j++) {
                            Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                            ScriptComponent* sc = GetComponent(e, ScriptComponent);
                            if (sc->mousebutton) sc->mousebutton(e, mb, INPUTDOWN);
                        }
                    }
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
                    ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                    if (scripts) {
                        for (size_t j = 0; j < scripts->size; j++) {
                            Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                            ScriptComponent* sc = GetComponent(e, ScriptComponent);
                            if (sc->mousemove) sc->mousemove(e, GetMousePosition());
                        }
                    }
                }
            }
            if (mscroll.x != 0 || mscroll.y != 0) {
                for (size_t i = 0; i < scene->worlds.size; i++) {
                    if (scene->worlds.data[i]->mousescroll)
                        scene->worlds.data[i]->mousescroll(scene->worlds.data[i], mscroll);
                    for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                        if (scene->worlds.data[i]->systems.data[j]->mousescroll)
                            scene->worlds.data[i]->systems.data[j]->mousescroll(scene->worlds.data[i]->systems.data[j], mscroll);
                    ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                    if (scripts) {
                        for (size_t j = 0; j < scripts->size; j++) {
                            Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                            ScriptComponent* sc = GetComponent(e, ScriptComponent);
                            if (sc->mousescroll) sc->mousescroll(e, mscroll);
                        }
                    }
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
                        ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[j], ScriptComponent);
                        if (scripts) {
                            for (size_t k = 0; k < scripts->size; k++) {
                                Entity e = (Entity){ scripts->data[k], scene->worlds.data[j] };
                                ScriptComponent* sc = GetComponent(e, ScriptComponent);
                                if (sc->key) {
                                    sc->key(e, g_application.keylist.data[i], INPUTDOWN);
                                    if (IsKeyPressed(g_application.keylist.data[i]))
                                        sc->key(e, g_application.keylist.data[i], INPUTPRESS);
                                }

                            }
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
                        ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[j], ScriptComponent);
                        if (scripts) {
                            for (size_t k = 0; k < scripts->size; k++) {
                                Entity e = (Entity){ scripts->data[k], scene->worlds.data[j] };
                                ScriptComponent* sc = GetComponent(e, ScriptComponent);
                                if (sc->key) sc->key(e, g_application.keylist.data[i], INPUTRELEASE);
                            }
                        }
                    }
                }
            }
            for (size_t i = 0; i < scene->worlds.size; i++) {
                if (scene->worlds.data[i]->update)
                    scene->worlds.data[i]->update(scene->worlds.data[i], GetFrameTime() * (g_fastforward ? Config()->ffspeed : 1.0f));
                for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                    if (scene->worlds.data[i]->systems.data[j]->update)
                        scene->worlds.data[i]->systems.data[j]->update(scene->worlds.data[i]->systems.data[j], GetFrameTime() * (g_fastforward ? Config()->ffspeed : 1.0f));
                ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                if (scripts) {
                    for (size_t j = 0; j < scripts->size; j++) {
                        Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                        ScriptComponent* sc = GetComponent(e, ScriptComponent);
                        if (!sc->initialized && sc->init) { sc->init(e); sc->initialized = TRUE; }
                        if (sc->update) sc->update(e, GetFrameTime() * (g_fastforward ? Config()->ffspeed : 1.0f));
                    }
                }
            }
        }
        PreRenderUI(g_application.ui);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawUI(g_application.ui, 0, 0, GetScreenWidth(), GetScreenHeight());
        EndDrawing();
        for (size_t i = 0; i < g_application.scenes.size; i++) {
            Scene* s = g_application.scenes.data[i];
            for (size_t j = 0; j < s->worlds.size; j++) {
                FlushRemovalQueue(s->worlds.data[j]);
            }
        }
    }
}

void DestroyApplication() {
    DestroyUI(g_application.ui);
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

Scene* GetActiveScene() {
    if (g_application.scenes.size > 0) return g_application.scenes.data[g_application.current];
    return NULL;
}

BOOL Playing() {
    return g_playing;
}

void Pause() {
    g_playing = FALSE;
    g_steps = 0;
}

void Resume() {
    g_playing = TRUE;
}

BOOL IsFast() {
    return g_fastforward;
}

void ToggleFastForward() {
    g_fastforward = !g_fastforward;
}

void Step(size_t steps) {
    Resume();
    g_steps = steps + 1;
}
