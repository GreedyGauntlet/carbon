#include "application.h"
#include "data/definitions.h"
#include "core/entrypoint.h"
#include "core/scene.h"
#include "core/world.h"
#include "ui/panels/viewport.h"
#include "ui/panels/scenes.h"
#include "ui/panels/edit.h"
#include "ui/panels/assets.h"
#include "ui/panels/scripts.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "util/pick.h"
#include <data/colors.h>
#include <data/fonts.h>
#include <core/binds.h>
#include <core/config.h>
#include <ui/panels/console.h>
#include <ui/panels/graph.h>
#include <ui/notification.h>
#include <util/logger.h>
#include <raymath.h>

static Application g_application = { 0 };
static ARRLIST_StaticString g_scene_names = { 0 };
static size_t g_resolution_width = 1600;
static size_t g_resolution_height = 900;
static Vector2 g_windowsize = { -1.0f, -1.0f };
static BOOL g_playing = FALSE;
static BOOL g_fastforward = FALSE;
static size_t g_steps = 0;
static BOOL g_vsync = TRUE;
static ARRLIST_Panel g_shared_panels = { 0 };

static void ApplicationResized() {
    if ((g_windowsize.x == -1.0f && g_windowsize.y == -1.0f) ||
        (g_windowsize.x != GetScreenWidth() || g_windowsize.y != GetScreenHeight())) {
        g_windowsize.x = GetScreenWidth();
        g_windowsize.y = GetScreenHeight();
        ResizeUI(g_application.ui);
        ResizePickTarget();
    }
}

static void SetupDefaultConfig() {
    ConfigSetFloat("ffspeed", 2.0f);
    ConfigSetSize("stepsize", 1);
    ConfigSetCamera("camera", (Camera2D){ (Vector2){ 0, 0 }, (Vector2){ 0, 0 }, 0, 1.0f });
    ConfigSetBool("limitlogs", FALSE);
    ConfigSetMessageLevel("logfilter", LEVEL_NONE);
    ConfigSetBool("printlogs", TRUE);
    ConfigSetBool("echologs", TRUE);
    ConfigSetMessageLevel("notificationfilter", LEVEL_NONE);
    ConfigSetBool("enablenotifications", TRUE);
    ConfigSetBool("logsnotify", TRUE);
    ConfigSetBool("flipnotifications", FALSE);
    ConfigSetString("activescene", "");
    ConfigSetBool("startupscene", TRUE);
    ConfigSetBool("vsync", TRUE);
    ConfigSetSize("selectedentity", 0);
    ConfigSetSize("selectedworld", 0);
    ConfigSetBool("startupentity", TRUE);
    ConfigSetBool("enableclickselection", TRUE);
}

void InitializeApplication() {
    #ifndef PROD_BUILD
    g_application.memory = EZ_ALLOCATED();
    #endif
    SetupDefaultConfig();
    InitConfig();
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(g_resolution_width, g_resolution_height, g_application.name == NULL ? "Carbon Engine" : g_application.name);
    InitAudioDevice();
    InitializeInput();
    InitializeColors();
    InitializeFonts();
    #ifdef CARBON_RELEASE
        ARRLIST_Panel_add(&g_shared_panels, GenerateViewportPanel());
        ARRLIST_UIConfig default_config = { 0 };
	    ARRLIST_UIConfig_add(&default_config, (UIConfig){"Viewport", 0.0f, FALSE, FALSE, FALSE, FALSE});
        SetUIConfig(&default_config);
        ARRLIST_UIConfig_clear(&default_config);
        RefreshUI(&(g_application.ui), g_shared_panels);
    #else
        ARRLIST_Panel_add(&g_shared_panels, GenerateEditPanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateConsolePanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateViewportPanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateScenesPanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateAssetsPanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateScriptsPanel());
        ARRLIST_Panel_add(&g_shared_panels, GenerateGraphPanel());
        ARRLIST_UIConfig default_config = { 0 };
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){{ 0 }, 1250.0f, FALSE, TRUE, TRUE, FALSE}); // root
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){{ 0 }, 350.0f, FALSE, TRUE, TRUE, FALSE}); // [ scenes + assets + scripts | graph ] | viewport container
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){{ 0 }, GetScreenHeight() - 420.0f, TRUE, TRUE, TRUE, FALSE}); // scenes + assets + ascripts | graph container
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Scenes", 0.0f, FALSE, FALSE, FALSE, TRUE}); // scenes +
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Assets", 0.0f, FALSE, FALSE, FALSE, TRUE}); // + assets +
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Scripts", 0.0f, FALSE, FALSE, FALSE, FALSE}); // + scripts
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Profiling", 0.0f, FALSE, FALSE, FALSE, FALSE}); // graph
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Viewport", 0.0f, FALSE, FALSE, FALSE, FALSE}); // viewport
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){{ 0 }, GetScreenHeight() - 360.0f, TRUE, TRUE, TRUE, FALSE}); // edit | console container
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Edit", 0.0f, FALSE, FALSE, FALSE, FALSE}); // edit
    	ARRLIST_UIConfig_add(&default_config, (UIConfig){"Console", 0.0f, FALSE, FALSE, FALSE, FALSE}); // console
        SetUIConfig(&default_config);
        ARRLIST_UIConfig_clear(&default_config);
        LoadUIConfig(&(g_application.ui), g_shared_panels);
    #endif
    SetViewportCamera(ConfigGetCamera("camera"));
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
    #ifdef CARBON_RELEASE
        Resume();
    #endif
    if (ConfigGetBool("startupscene") && HasScene(ConfigGetString("activescene"))) {
        SetScene(ConfigGetString("activescene"));
    } else if (ConfigGetBool("startupscene")) {
        logwarn("Unable to override startup scene");
    }
    if (ConfigGetBool("startupentity")) {
        if (GetActiveScene() && ConfigGetSize("selectedworld") < GetActiveScene()->worlds.size) {
            SelectEntity((Entity){ ConfigGetSize("selectedentity"), GetActiveScene()->worlds.data[ConfigGetSize("selectedworld")] });
        } else {
            logwarn("Unable to find previously selected entity");
        }
    }
    while(!WindowShouldClose()) {
        if (g_vsync != ConfigGetBool("vsync")) {
            g_vsync = ConfigGetBool("vsync");
			if (ConfigGetBool("vsync")) {
                SetWindowState(FLAG_VSYNC_HINT);
                logtrace("VSYNC on");
            } else {
                ClearWindowState(FLAG_VSYNC_HINT);
                logtrace("VSYNC off");
            }
        }
        ApplicationResized();
        PreupdateExtensions();
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
                            Script* sc = EntityScript(e);
                            if (sc && sc->mousebutton) sc->mousebutton(e, mb, INPUTPRESS);
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
                            Script* sc = EntityScript(e);
                            if (sc && sc->mousebutton) sc->mousebutton(e, mb, INPUTRELEASE);
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
                            Script* sc = EntityScript(e);
                            if (sc && sc->mousebutton) sc->mousebutton(e, mb, INPUTDOWN);
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
                            Script* sc = EntityScript(e);
                            if (sc && sc->mousemove) sc->mousemove(e, GetMousePosition());
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
                            Script* sc = EntityScript(e);
                            if (sc && sc->mousescroll) sc->mousescroll(e, mscroll);
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
                                Script* sc = EntityScript(e);
                                if (sc && sc->key) {
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
                                Script* sc = EntityScript(e);
                                if (sc && sc->key) sc->key(e, g_application.keylist.data[i], INPUTRELEASE);
                            }
                        }
                    }
                }
            }
            float ffspeed = ConfigGetFloat("ffspeed");
            for (size_t i = 0; i < scene->worlds.size; i++) {
                if (scene->worlds.data[i]->update)
                    scene->worlds.data[i]->update(scene->worlds.data[i], GetFrameTime() * (g_fastforward ? ffspeed : 1.0f));
                for (size_t j = 0; j < scene->worlds.data[i]->systems.size; j++)
                    if (scene->worlds.data[i]->systems.data[j]->update)
                        scene->worlds.data[i]->systems.data[j]->update(scene->worlds.data[i]->systems.data[j], GetFrameTime() * (g_fastforward ? ffspeed : 1.0f));
                ARRLIST_EntityID* scripts = GetEntities(scene->worlds.data[i], ScriptComponent);
                if (scripts) {
                    for (size_t j = 0; j < scripts->size; j++) {
                        Entity e = (Entity){ scripts->data[j], scene->worlds.data[i] };
                        Script* sc = EntityScript(e);
                        if (sc && !sc->initialized && sc->init) { sc->init(e); sc->initialized = TRUE; }
                        if (sc && sc->update) sc->update(e, GetFrameTime() * (g_fastforward ? ffspeed : 1.0f));
                    }
                }
            }
        }
        if (!UIRequestsBlockInput()) ListenBinds();
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
        RefreshUI(&(g_application.ui), g_shared_panels);
    }
}

void DestroyApplication() {
    CleanPicking();
    #ifndef CARBON_RELEASE
        SaveUIConfig(g_application.ui);
    #endif
    if (GetActiveScene()) {
        ConfigSetString("activescene", GetActiveScene()->name);
    }
    ConfigSetSize("selectedworld", 0);
    Entity e = SelectedEntity();
    ConfigSetSize("selectedentity", e.id);
    for (size_t i = 0; GetActiveScene() && i < GetActiveScene()->worlds.size; i++) {
        if (GetActiveScene()->worlds.data[i] == e.context) {
            ConfigSetSize("selectedworld", i);
            break;
        }
    }
    ConfigSetCamera("camera", GetViewportCamera());
    CleanConfig();
    ARRLIST_StaticString_clear(&g_scene_names);
    CleanBinds();
    DestroyUI(g_application.ui);
	for (size_t i = 0; i < g_shared_panels.size; i++) DestroyPanel(&(g_shared_panels.data[i]));
    CleanConsoleLogs();
    CloseAudioDevice();
    CloseWindow();
    HASHMAP_KeyMap_clear(&g_application.keymap);
    ARRLIST_int_clear(&g_application.keylist);
    for (size_t i = 0; i < g_application.scenes.size; i++) DestroyScene(g_application.scenes.data[i]);
    ARRLIST_ScenePtr_clear(&g_application.scenes);
    ARRLIST_Panel_clear(&g_shared_panels);
    CleanNotifications();
    CleanupExtensions();
    #ifndef PROD_BUILD
    EZ_ASSERT(EZ_ALLOCATED() == 0, "Memory cleanup revealed a leak of %d bytes", (int)(EZ_ALLOCATED() - g_application.memory));
    #endif
    EZ_INFO("%s", g_application.goodbye == NULL ? "Hoc est omnia, amice." : g_application.goodbye);
}

void AddScene(Scene* scene) {
    ARRLIST_ScenePtr_add(&g_application.scenes, scene);
    ARRLIST_StaticString_add(&g_scene_names, scene->name);
}

BOOL HasScene(const char* name) {
    for (size_t i = 0; i < g_application.scenes.size; i++) {
        if (strcmp(g_application.scenes.data[i]->name, name) == 0) {
            return TRUE;
        }
    }
    return FALSE;
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
    if (GetActiveScene()) {
        Scene* scene = GetActiveScene();
        for (size_t i = 0; i < scene->worlds.size; i++) {
            ARRLIST_EntityID* cameras = GetEntities(scene->worlds.data[i], CameraComponent);
            int numc = 0;
            for (size_t j = 0; cameras && j < cameras->size; j++) {
                Entity e = (Entity){ cameras->data[j], scene->worlds.data[i] };
                if (GetComponent(e, CameraComponent)->enabled) numc++;
            }
            if (numc == 0) {
                logwarn("No active camera component detected detected in the current scene's %dth world", (int)i);
            } else if (numc > 1) {
                logwarn("More than one active camera component detected detected in the current scene's %dth world", (int)i);
            }
        }
    } else {
        logwarn("No active scene to play!");
    }
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

ARRLIST_StaticString* SceneNames() {
    return &g_scene_names;
}

ARRLIST_Panel* EditorPanels() {
    return &g_shared_panels;
}

Vector2 GameMousePosition() {
    return Vector2Subtract(GetMousePosition(), GetViewportPosition());
}
