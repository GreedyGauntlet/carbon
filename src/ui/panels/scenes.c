#include "scenes.h"
#include "data/definitions.h"
#include "ui/ui.h"

static void DrawScenesPanel(float width, float height) {
    UIDrawText("TODO");
}

Panel GenerateScenesPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Scenes");
	p.draw = DrawScenesPanel;
	return p;
}
