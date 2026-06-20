#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

typedef void (*EntryPoint)(void);

#define REGISTER_PRELOAD(func) __attribute__((section("preloads"))) \
    EntryPoint ext = func;

void PreloadExtensions();

#endif
