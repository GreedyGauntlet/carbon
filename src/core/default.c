#include "default.h"
#include "commands/configure.h"
#include "commands/timeevent.h"
#include "commands/help.h"
#include "commands/notify.h"

// TODO: delete this!!!
#include "core/scene.h"
#include "core/world.h"
#include "core/application.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "util/logger.h"
#include <easylogger.h>
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
    PackTexture(scene, LoadTexture("assets/textures/default.png"), "Default Square");
    Entity e = CreateEntity(world);
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){255,0,0,255});
    AddComponent(e, MusicComponent, PackMusic(scene, LoadMusicStream("assets/audio/sea_shanties.mp3"), "Fart Noise"), 1.0f, 1.0f, 0.0, AUDIO_NOTHING);
    AddComponent(e, ScriptComponent, PackScript(scene, (Script){NULL, Fart, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE}, "Fart Script", "Triggers a fart on spacebar"));
    for (size_t i = 0; i < 50; i++) {
        e = CreateEntityP(world, 75 + i * 50, 25 + i * 50, 1);
        EntityScale(e)->x = 100;
        EntityScale(e)->y = 100;
        AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){0,0,255,255});
    }
    e = CreateEntityP(world, 0, 0, 0.5f);
    AddComponent(e, TextComponent, "TEST TEST TEST", TEXT_ALIGN_CENTER, (Color){255,255,255,255}, 20.0f);
    AddComponent(e, AnchorComponent, CENTER_ANCHOR);
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
}

void DefaultPreupdate() { }

void DefaultCleanup() { }

REGISTER_PRELOAD(DefaultPreload);
REGISTER_POSTLOAD(DefaultPostload);
REGISTER_POSTLOAD(DevTest);
REGISTER_PREUPDATE(DefaultPreupdate);
REGISTER_CLEANUP(DefaultCleanup);
