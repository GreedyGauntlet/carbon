#include "family.h"
#include "data/definitions.h"
#include "systems/system.h"
#include "ecs/entity.h"
#include "ecs/components.h"

static void UpdateFamilySystem(System* system, float dt) {

}
static void CleanFamilySystem(System* system) {

}

System* GenerateFamilySystem() {
    return GenerateSystem(NULL, UpdateFamilySystem, NULL, NULL, NULL, NULL, CleanFamilySystem);
}

ARRLIST_EntityID GetChildren(Entity e) {

}

// Next:
// 1. create easyC sets
// 2. in registry track alive entities via a set
// 3. make entity exists function
// 4. system has a hasmap of parents to arrlists of children
// 5. on get children ensure all entities exist, remove all of those that dont
// 6. ^ don't forget to do this efficiently with a mark-copy-restore call instead of many remove calls
// 7. make get recursive global transform function to traverse parents
// 8. use that in draw system properly
// 9. yay parent component implemented
// 10. work on scene management panel
// 11. implement scene switching
// 12. implement entity list
// 13. make sure you calculate properly so only loop through and list entities that are visible
// 14. gah make it scrollable again
// 15. keep track of selected entity
// 16. done with sprint!
