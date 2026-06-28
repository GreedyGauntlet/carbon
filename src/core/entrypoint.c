#include "entrypoint.h"

extern EntryPoint __start_preloads[];
extern EntryPoint __stop_preloads[];
extern EntryPoint __start_postloads[];
extern EntryPoint __stop_postloads[];
extern EntryPoint __start_preupdates[];
extern EntryPoint __stop_preupdates[];
extern EntryPoint __start_cleanups[];
extern EntryPoint __stop_cleanups[];

void PreloadExtensions() {
    for (EntryPoint *p = __start_preloads; p < __stop_preloads; p++) (*p)();
}

void PostloadExtensions() {
    for (EntryPoint *p = __start_postloads; p < __stop_postloads; p++) (*p)();
}

void PreupdateExtensions() {
    for (EntryPoint *p = __start_preupdates; p < __stop_preupdates; p++) (*p)();
}

void CleanupExtensions() {
    for (EntryPoint *p = __start_cleanups; p < __stop_cleanups; p++) (*p)();
}
