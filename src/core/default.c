#include "default.h"
#include "commands/configure.h"
#include "commands/timeevent.h"
#include "commands/help.h"

// TODO: delete this!!!
#include "core/scene.h"
#include "core/world.h"
#include "core/application.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "util/logger.h"
#include <easylogger.h>
static void Move(const Entity e, float dt){
    EntityPosition(e)->x += dt * 100.0f;
    EntityPosition(e)->y += dt * 50.0f;
}
static void InitDev(Scene* scene) {
    World* world = GenerateWorld(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    AddWorld(scene, world);
    Entity e = CreateEntity(world);
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){255,0,0,255});
    e = CreateEntityP(world, 0, 50, -1);
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){0,255,0,255});
    AddComponent(e, ScriptComponent, NULL, Move, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE);
    e = CreateEntityP(world, 75, 25, 1);
    EntityScale(e)->x = 100;
    EntityScale(e)->y = 100;
    AddComponent(e, ShapeComponent, CIRCLE_SHAPE, (Color){0,0,255,255});
    e = CreateEntityP(world, 0, 0, 0.5f);
    AddComponent(e, TextComponent, "TEST TEST TEST", TEXT_ALIGN_CENTER, (Color){255,255,255,255}, 20.0f);
    AddComponent(e, AnchorComponent, CENTER_ANCHOR);
    e = CreateEntityP(world, 0, 0, 100);
    AddComponent(e, CameraComponent, TRUE, (Vector2){ 0, 0 }, 45, 2.0f);
    loginfo("Success?");
}
static void CleanDev(Scene* scene) {
    WipeScene(scene);
}
static void DevTest() {
    Scene* scene = GenerateScene("Development", InitDev, CleanDev);
    AddScene(scene);
}

void DefaultPreload() { }

void DefaultPostload() {
    // register config commands here
    RegisterCommand((Command){ "help", HelpCommand, "help {usage|descriptions}", "Outputs help on console commands"});
    RegisterCommand((Command){ "config", ConfigureCommand, "config {read|write} {<param>} (<value>)", "Read or write to the editor config" });
    RegisterCommand((Command){ "timeevent", TimeEventCommand, "timeevent {<seconds>} ...", "Set a command to execute in a given amount of time" });
}

void DefaultPreupdate() { }

void DefaultCleanup() { }

REGISTER_PRELOAD(DefaultPreload);
REGISTER_POSTLOAD(DefaultPostload);
REGISTER_POSTLOAD(DevTest);
REGISTER_PREUPDATE(DefaultPreupdate);
REGISTER_CLEANUP(DefaultCleanup);
