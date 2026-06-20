#include "entrypoint.h"

extern EntryPoint __start_preloads[];
extern EntryPoint __stop_preloads[];

void PreloadExtensions() {
    for (EntryPoint *p = __start_preloads; p < __stop_preloads; p++) (*p)();
}
