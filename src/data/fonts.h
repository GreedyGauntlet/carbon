#ifndef FONTS_H
#define FONTS_H

#include <raylib.h>

#define MAX_FONT_SIZE 512

void InitializeFonts();

Font FontAsset();

Font FontAssetItalic();

void DestroyFonts();

#endif
