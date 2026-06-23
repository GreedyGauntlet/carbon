#include "default.h"

// TODO: delete this!!!
#include "core/scene.h"
#include "core/world.h"
#include "core/application.h"
#include "ecs/entity.h"
#include "ecs/components.h"
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
    EZ_INFO("Success?");
}
static void CleanDev(Scene* scene) {
    WipeScene(scene);
}
static void DevTest() {
    Scene* scene = GenerateScene("Development", InitDev, CleanDev);
    AddScene(scene);
}

void DefaultEntryPoint() {}

REGISTER_PRELOAD(DefaultEntryPoint);
REGISTER_POSTLOAD(DefaultEntryPoint);
REGISTER_POSTLOAD(DevTest);
