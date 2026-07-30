#include "application.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "data/fonts.h"
#include "core/entrypoint.h"
#include "core/config.h"
#include "core/scene.h"
#include "core/world.h"
#include "core/binds.h"
#include "ui/panels/viewport.h"
#include "ui/panels/console.h"
#include "ui/panels/scenes.h"
#include "ui/panels/graph.h"
#include "ui/panels/edit.h"
#include "ui/panels/assets.h"
#include "ui/panels/scripts.h"
#include "ui/notification.h"
#include "ecs/components.h"
#include "ecs/entity.h"
#include "util/logger.h"
#include "util/pick.h"

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
static ARRLIST_UIConfig g_ui_config = { 0 };
static BOOL g_reset_ui = FALSE;

static void ApplicationResized() {
    if ((g_windowsize.x == -1.0f && g_windowsize.y == -1.0f) ||
        (g_windowsize.x != GetScreenWidth() || g_windowsize.y != GetScreenHeight())) {
        g_windowsize.x = GetScreenWidth();
        g_windowsize.y = GetScreenHeight();
        ResizeUI(g_application.ui);
        ResizePickTarget();
    }
}

static void ResetDefaultConfig() {
    ARRLIST_UIConfig_clear(&g_ui_config);
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){{ 0 }, 1250.0f, FALSE, TRUE, TRUE, FALSE}); // root
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){{ 0 }, 350.0f, FALSE, TRUE, TRUE, FALSE}); // [ scenes + assets + scripts | graph ] | viewport container
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){{ 0 }, GetScreenHeight() - 420.0f, TRUE, TRUE, TRUE, FALSE}); // scenes + assets + ascripts | graph container
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Scenes", 0.0f, FALSE, FALSE, FALSE, TRUE}); // scenes +
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Assets", 0.0f, FALSE, FALSE, FALSE, TRUE}); // + assets +
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Scripts", 0.0f, FALSE, FALSE, FALSE, FALSE}); // + scripts
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Profiling", 0.0f, FALSE, FALSE, FALSE, FALSE}); // graph
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Viewport", 0.0f, FALSE, FALSE, FALSE, FALSE}); // viewport
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){{ 0 }, GetScreenHeight() - 360.0f, TRUE, TRUE, TRUE, FALSE}); // edit | console container
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Edit", 0.0f, FALSE, FALSE, FALSE, FALSE}); // edit
	ARRLIST_UIConfig_add(&g_ui_config, (UIConfig){"Console", 0.0f, FALSE, FALSE, FALSE, FALSE}); // console
}

static void LoadUIConfigHelper(UI** current, size_t* index) {
	UIConfig conf = g_ui_config.data[*index];
	if (*current == NULL) *current = GenerateUI();
	if (conf.left || conf.right) {
		(*current)->divide = conf.divide;
		(*current)->vertical = conf.vertical;
		if (conf.left) {
			*index += 1;
			LoadUIConfigHelper((UI**)&((*current)->left), index);
		}
		if (conf.right) {
			*index += 1;
			LoadUIConfigHelper((UI**)&((*current)->right), index);
		}
	} else {
		for (size_t i = 0; i < g_shared_panels.size; i++) {
			if (strcmp(g_shared_panels.data[i].name, conf.name) == 0) {
				ARRLIST_Panel_add(&((*current)->panels), g_shared_panels.data[i]);
				if (conf.vine) {
					*index += 1;
					LoadUIConfigHelper(current, index);
				}
				return;
			}	
		}
		logwarn("Unable to find and set up matching panel \"%s\"", conf.name);
	}
}

static void LoadUIConfig() {
    ResetDefaultConfig();
    FILE* file = fopen(".uiconf", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        if ((filesize - sizeof(size_t)) % sizeof(UIConfig) == 0) {
            rewind(file);
            size_t count = 0;
            fread(&count, 1, sizeof(size_t), file);
            if (count > 0) {
                if ((size_t)filesize == sizeof(size_t) + count * sizeof(UIConfig)) {
                    ARRLIST_UIConfig_clear(&g_ui_config);
                    for (size_t i = 0; i < count; i++) {
                        UIConfig conf = { 0 };
                        fread(&conf, 1, sizeof(UIConfig), file);
                        ARRLIST_UIConfig_add(&g_ui_config, conf);
                    }
                    logtrace("Successfully loaded ui config");
                } else {
                    logwarn("Desynced panel count detected - falling back to default config");
                }
            } else {
                logwarn("No configured panels detected - falling back to default config");
            }
        } else {
            logwarn("Existing ui config is invalid - falling back to default config");
        }
        fclose(file);
    } else {
        logtrace("Unable to detect existing ui config - using default config");
    }
	size_t i = 0;
    LoadUIConfigHelper(&(g_application.ui), &i);
    SetPrimaryUI(g_application.ui);
}

static void CopyUIDividersToConfigHelper(UI* ui, size_t* index) {
    g_ui_config.data[*index].divide = ui->divide;
    if (ui->left) {
        *index += 1;
        CopyUIDividersToConfigHelper(GetLeftUI(ui), index);
    }
    if (ui->right) {
        *index += 1;
        CopyUIDividersToConfigHelper(GetRightUI(ui), index);
    }
    if (ui->panels.size > 0 ) *index += ui->panels.size - 1;
}

static void CopyUIDividersToConfig() {
    size_t i = 0;
    CopyUIDividersToConfigHelper(g_application.ui, &i);
}

static void SaveUIConfig() {
    FILE *file = fopen(".uiconf", "wb");
    if (file) {
        fwrite(&g_ui_config.size, 1, sizeof(size_t), file);
        for (size_t i = 0; i < g_ui_config.size; i++)
            fwrite(&(g_ui_config.data[i]), 1, sizeof(UIConfig), file);
        fclose(file);
    }
}

void InitializeApplication() {
    #ifndef PROD_BUILD
    g_application.memory = EZ_ALLOCATED();
    #endif
    InitConfig();
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(g_resolution_width, g_resolution_height, g_application.name == NULL ? "Carbon Engine" : g_application.name);
    InitAudioDevice();
    InitializeInput();
    InitializeColors();
    InitializeFonts();
    ARRLIST_Panel_add(&g_shared_panels, GenerateEditPanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateConsolePanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateViewportPanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateScenesPanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateAssetsPanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateScriptsPanel());
    ARRLIST_Panel_add(&g_shared_panels, GenerateGraphPanel());
    LoadUIConfig();
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
    if (Config()->startupscene && HasScene(Config()->activescene)) {
        SetScene(Config()->activescene);
    } else if (Config()->startupscene) {
        logwarn("Unable to override startup scene");
    }
    if (Config()->startupentity) {
        if (Config()->selectedworld < GetActiveScene()->worlds.size) {
            SelectEntity((Entity){ Config()->selectedentity, GetActiveScene()->worlds.data[Config()->selectedworld] });
        } else {
            logwarn("Unable to find previously selected entity");
        }
    }
    while(!WindowShouldClose()) {
        if (g_vsync != Config()->vsync) {
            g_vsync = Config()->vsync;
			if (Config()->vsync) {
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
                        Script* sc = EntityScript(e);
                        if (sc && !sc->initialized && sc->init) { sc->init(e); sc->initialized = TRUE; }
                        if (sc && sc->update) sc->update(e, GetFrameTime() * (g_fastforward ? Config()->ffspeed : 1.0f));
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
        if (g_reset_ui){
            g_reset_ui = FALSE;
            WipeUI(g_application.ui);
            g_application.ui = NULL;
            size_t i = 0;
            LoadUIConfigHelper(&(g_application.ui), &i);
            SetPrimaryUI(g_application.ui);
        }
    }
}

void DestroyApplication() {
    CleanPicking();
    CopyUIDividersToConfig();
    SaveUIConfig();
    CleanConfig();
    ARRLIST_StaticString_clear(&g_scene_names);
    CleanBinds();
    DestroyUI(g_application.ui);
	for (size_t i = 0; i < g_shared_panels.size; i++) DestroyPanel(&(g_shared_panels.data[i]));
    CloseAudioDevice();
    CloseWindow();
    HASHMAP_KeyMap_clear(&g_application.keymap);
    ARRLIST_int_clear(&g_application.keylist);
    for (size_t i = 0; i < g_application.scenes.size; i++) DestroyScene(g_application.scenes.data[i]);
    ARRLIST_ScenePtr_clear(&g_application.scenes);
    ARRLIST_Panel_clear(&g_shared_panels);
	ARRLIST_UIConfig_clear(&g_ui_config);
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

ARRLIST_UIConfig* GetUIConfig() {
    return &g_ui_config;
}

ARRLIST_Panel* EditorPanels() {
    return &g_shared_panels;
}

void SetUIConfig(ARRLIST_UIConfig* config) {
	ARRLIST_UIConfig_clear(&g_ui_config);
    for (size_t i = 0; i < config->size; i++) {
        ARRLIST_UIConfig_add(&g_ui_config, config->data[i]);
    }
    g_reset_ui = TRUE;
}
