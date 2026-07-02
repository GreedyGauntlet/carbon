#include "edit.h"
#include "data/definitions.h"
#include "ui/ui.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "core/application.h"
#include "core/world.h"

static Entity g_selected = { 0 };

static void DrawEditPanel(float width, float height) {
    if (g_selected.id == INVALID_ENTITY || !IsActiveWorld(g_selected.context) || !HasComponent(g_selected, TagComponent)) {
        UISetCursor(width / 2.0f - (UITextWidth("No Selected Entity") / 2.0f), height / 2.0f - 10.0f);
        UIDrawText("No Selected Entity");
        return;
    }
    // NAME: [ ....  ] [+]
    // -- transform -------------------------------
    // [ x] [ y] [z]
    // [w ] [ l]
    // [ rotatopnm] ( dial with line that rotates!)
    // -- anchor ---------------------------------- 
    // Anchor: [   anchor    ] [ square with red dot signifying anchor ]
    // -- image ----------------------------------- 
    // TODO! needs to implement assetpack first
    // Asset: [    asset     ] [ reload ]
    // -- animation -------------------------------- 
    // Asset: [    asset     ] [reload ]
    // Animation: [ Animaton ] 
    // -- listener ----------------------------------
    // Enabled: []
    // -- sound ------------------------------------
    // Asset [ asset ] [ reload ]
    // [ play / pause / resume ] [ stop ]
    // -- text --------------------------------------
    // text: [    text     ]
    // alightnment [ alightnment]
    // size: [ size ]
    // [ r ] [ g ] [b ] [a] 
    // -- camera ------------------------------------
    // enabled: []
    // [ x] [ y]
    // [ rotatopnm] ( dial with line that rotates!)
    // zoom : [ zoom ]
    // -- shape ------------------------------------- 
    // type [ type] 
    // [ r ] [ g ] [b ] [a] 
    // -- script ------------------------------------
    // script [    asset      ] [reload]

    // ASSETS
    // --------------------- 
    //
    //     view asset here
    //
    // --------------------] <- resizeable window
    // < scrollable list of assets >
    // SHOW: [ ] textures   [ ] animations   [ ] audio
    //  > click to select 
    //  |-> on triangle click open up info
    //      - path [reload]
    //      - type
    //      - image?
    //          - width
    //          - height
    //      - animation?
    //        - total width
    //          - total height
    //          - total frames
    //          - animations
    //              - name
    //                  - start frame
    //                  - end frame
    //                  - fps
    //                  - loop?
    //                  - flip?
    //                  - flop?
    //      - sound?
    //          - type? (sound or music)
    //          - framecount
    
    // SCRIPTS
    // idk list of scripts
    //  - you can open them for descriptions
}

Panel GenerateEditPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Edit");
	p.draw = DrawEditPanel;
	return p;
}

void SelectEntity(Entity e) {
    g_selected = e;
}

Entity SelectedEntity() {
    return g_selected;
}
