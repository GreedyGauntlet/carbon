#include "entrypoint.h"

extern EntryPoint __start_preloads[];
extern EntryPoint __stop_preloads[];
extern EntryPoint __start_postloads[];
extern EntryPoint __stop_postloads[];

void PreloadExtensions() {
    for (EntryPoint *p = __start_preloads; p < __stop_preloads; p++) (*p)();
}

void PostloadExtensions() {
    for (EntryPoint *p = __start_postloads; p < __stop_postloads; p++) (*p)();
}
