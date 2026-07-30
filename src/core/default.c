#include "default.h"
#include "commands/configure.h"
#include "commands/timeevent.h"
#include "commands/help.h"
#include "commands/notify.h"
#include "commands/clear.h"

// TODO: delete this!!!
#include "core/scene.h"
#include "core/world.h"
#include "core/application.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "util/logger.h"
#include <easylogger.h>
#include <math.h>
static void Move(const Entity e, float dt) {
    if (IsKeyDown(KEY_A)) {
        EntityPosition(e)->x -= dt * 500.0f;
    }
    if (IsKeyDown(KEY_D)) {
        EntityPosition(e)->x += dt * 500.0f;
    }
    if (IsKeyDown(KEY_S)) {
        EntityPosition(e)->y += dt * 500.0f;
    }
    if (IsKeyDown(KEY_W)) {
        EntityPosition(e)->y -= dt * 500.0f;
    }
}
static void Fart(const Entity e, float dt) {
    if (IsKeyPressed(KEY_SPACE)) {
        GetComponent(e, MusicComponent)->command = AUDIO_PLAY;
        logwarn("FARTING!!!");
    }
}
static void InitDev(Scene* scene) {
    World* world = GenerateWorld("Test World", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    AddWorld(scene, world);
    Script ms = (Script){NULL, Move, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE};
    size_t movescript = PackScript(scene, ms, "Move", "Test Description:\n\n1. This is a test description for a script that moves an entity in like uh a direction\n2.Yeah thats like it");
    PackTexture(scene, "assets/textures/default.png", "Default Square");
    PackTexture(scene, "assets/textures/SHEET_explosion.png", "Explosion Animation Sheet");
    PackSound(scene, "assets/audio/fart.mp3", "Fart Noise");
    PackMusic(scene, "assets/audio/sea_shanties.mp3", "Sea Shanty");
    PackShader(scene, NULL, "assets/shaders/cross_stitching.fs", "Cross Hatch");
    PackAnimation(scene, (Animation){FindTexture(scene, "Explosion Animation Sheet"), 17, 631, 418, (Vector2){0, 0}, 10.0f}, "Explosion");
    Entity e = CreateEntityN(world, "Explosionsssss");
    AddComponent(e, ShaderComponent, FindShader(scene, "Cross Hatch"));
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, AnimationComponent, FindAnimation(scene, "Explosion"), 0.0f, 1.0f, FALSE, TRUE, FALSE, FALSE);
    AddComponent(e, MusicComponent, FindMusic(scene, "Sea Shanty"), 1.0f, 1.0f, 0.0, AUDIO_NOTHING);
    AddComponent(e, ScriptComponent, PackScript(scene, (Script){NULL, Fart, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE}, "Fart Script", "Triggers a fart on spacebar"));
    for (size_t i = 0; i < 50; i++) {
        e = CreateEntityP(world, 75 + sin((float)i) * 90, 25 + cos((float)i) * 90, 1);
        EntityScale(e)->x = 10;
        EntityScale(e)->y = 10;
        AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){0,0,255,255});
    }
    e = CreateEntityNP(world, "Text Test Entity", 0, 0, 0.5f);
    AddComponent(e, TextComponent, "TEST TEST TEST", 0, TEXT_ALIGN_CENTER, (Color){255,255,255,255}, 20.0f);
    AddComponent(e, AnchorComponent, CENTER_ANCHOR);
    AddComponent(e, TextureComponent, FindTexture(scene, "Default Square"));
    AddComponent(e, AnimationComponent, FindAnimation(scene, "Explosion"), 0.0f, 1.0f, FALSE, TRUE, FALSE, FALSE);
    AddComponent(e, ListenerComponent, FALSE, 1.0, 0.5);
    AddComponent(e, SoundComponent, FindSound(scene, "Fart Noise"), 1.0, 1.0, 0.5, AUDIO_NOTHING);
    AddComponent(e, MusicComponent, FindMusic(scene, "Sea Shanty"), 1.0f, 1.0f, 0.0, AUDIO_NOTHING);
    AddComponent(e, CameraComponent, FALSE, (Vector2){ 0, 0 }, 0, 1.0f);
    AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){0,0,255,255});
    //AddComponent(e, ScriptComponent, movescript);
    e = CreateEntityP(world, 0, 50, -1);
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, TextureComponent, FindTexture(scene, "Default Square"));
    AddComponent(e, ScriptComponent, movescript);
    AddComponent(e, ListenerComponent, TRUE, 1.0, 0.5);
    Entity c = CreateEntityNP(world, "I have a parent", 0, 0, 100);
    AddComponent(c, CameraComponent, TRUE, (Vector2){ 0, 0 }, 0, 1.0f);
    LinkFamily(e, c);
    loginfo("Successfully set up test scene");
}
static void CleanDev(Scene* scene) {
    WipeScene(scene);
}
static void DevTest() {
    Scene* scene = GenerateScene("Development", InitDev, CleanDev);
    AddScene(scene);
    Scene* scene2 = GenerateScene("Development 2", InitDev, CleanDev);
    AddScene(scene2);
    Scene* scene3 = GenerateScene("Development 3", InitDev, CleanDev);
    AddScene(scene3);
}

void DefaultPreload() { }

void DefaultPostload() {
    // register config commands here
    RegisterCommand((Command){ "help", HelpCommand, "help {usage|descriptions}", "Outputs help on console commands"});
    RegisterCommand((Command){ "config", ConfigureCommand, "config {read|write} {<param>} (<value>)", "Read or write to the editor config" });
    RegisterCommand((Command){ "timeevent", TimeEventCommand, "timeevent {<seconds>} ...", "Set a command to execute in a given amount of time" });
    RegisterCommand((Command){ "notify", NotifyCommand, "notify <level> ...", "Send a notification of a given level and with the given text" });
    RegisterCommand((Command){ "clear", ClearCommand, "clear {notifications}", "Clear a given data subject" });
}

void DefaultPreupdate() { }

void DefaultCleanup() { }

REGISTER_PRELOAD(DefaultPreload);
REGISTER_POSTLOAD(DefaultPostload);
REGISTER_POSTLOAD(DevTest);
REGISTER_PREUPDATE(DefaultPreupdate);
REGISTER_CLEANUP(DefaultCleanup);
