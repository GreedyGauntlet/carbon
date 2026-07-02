#include "assets.h"
#include "data/definitions.h"
#include "ui/ui.h"

static void DrawAssetsPanel(float width, float height) {
    UIDrawText("TODO");
 }

Panel GenerateAssetsPanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Assets");
	p.draw = DrawAssetsPanel;
	return p;
}
