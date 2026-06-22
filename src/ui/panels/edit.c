#include "edit.h"
#include "data/definitions.h"
#include "ui/ui.h"

static void DrawEditPanel(float width, float height) {
    UIDrawText("TODO");
}

Panel GenerateEditPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Edit");
	p.draw = DrawEditPanel;
	return p;
}
