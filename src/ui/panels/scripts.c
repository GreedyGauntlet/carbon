#include "scripts.h"
#include "data/definitions.h"
#include "ui/ui.h"

static void DrawScriptsPanel(float width, float height) {
    UIDrawText("TODO");
 }

Panel GenerateScriptsPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Scripts");
	p.draw = DrawScriptsPanel;
	return p;
}
