#include "fonts.h"
#include <stddef.h>

static Font g_font;
static Font g_font_italic;

void InitializeFonts() {
    g_font = LoadFontEx("assets/fonts/OpenSans-Regular.ttf", MAX_FONT_SIZE, NULL, 0);
    g_font_italic = LoadFontEx("assets/fonts/OpenSans-Italic.ttf", MAX_FONT_SIZE, NULL, 0);
}

Font FontAsset() {
    return g_font;
}

Font FontAssetItalic() {
    return g_font_italic;
}

void DestroyFonts() {
    UnloadFont(g_font);
    UnloadFont(g_font_italic);
}
