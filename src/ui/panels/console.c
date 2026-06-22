#include "console.h"
#include "data/definitions.h"
#include "ui/ui.h"

static void DrawConsolePanel(float width, float height) {
    UIDrawText("TODO");
}

Panel GenerateConsolePanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Console");
	p.draw = DrawConsolePanel;
	return p;
}
