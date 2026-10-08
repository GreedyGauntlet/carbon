#include "select.h"
#include "ecs/entity.h"
#include "core/world.h"
#include "core/scene.h"
#include "core/application.h"
#include "util/logger.h"
#include "ui/panels/edit.h"
#include <easyparse.h>

BOOL SelectCommand(char** argv, int argc) {
    Scene* scene = GetActiveScene();
    if (!scene) {
        logerror("No active scene to select an entity from");
        return FALSE;
    }
    if (argc != 2) {
        logerror("This command requires 2 arguments - the world name and entity ID");
        return FALSE;
    }
    World* world = FindWorld(scene, argv[0]);
    if (!world) {
        logerror("No active world under the name \"%s\" detected - unable to select entity", argv[0]);
        return FALSE;
    }
    uint32_t id;
    if (!ez_parse_uint(argv[1], &id)) {
        logerror("Unable to parse \"%s\" as an ID", argv[1]);
        return FALSE;
    }
    Entity e = (Entity){ id, world };
    SelectEntity(e);
    loginfo("Successfully selected entity");
    return TRUE;
}
