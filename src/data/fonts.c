#include "fonts.h"
#include <stddef.h>

static Font g_font;

void InitializeFonts() {
    g_font = LoadFontEx("assets/fonts/OpenSans-Regular.ttf", MAX_FONT_SIZE, NULL, 0);
}

Font FontAsset() {
    return g_font;
}

void DestroyFonts() {
    UnloadFont(g_font);
}
