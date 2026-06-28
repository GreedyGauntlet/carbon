#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

typedef void (*EntryPoint)(void);

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define REGISTER_PRELOAD(func) \
    __attribute__((section("preloads"))) \
    EntryPoint CONCAT(ext_, __COUNTER__) = func;

#define REGISTER_POSTLOAD(func) \
    __attribute__((section("postloads"))) \
    EntryPoint CONCAT(ext_, __COUNTER__) = func;

#define REGISTER_PREUPDATE(func) \
    __attribute__((section("preupdates"))) \
    EntryPoint CONCAT(ext_, __COUNTER__) = func;

#define REGISTER_CLEANUP(func) \
    __attribute__((section("cleanups"))) \
    EntryPoint CONCAT(ext_, __COUNTER__) = func;

void PreloadExtensions();

void PostloadExtensions();

void PreupdateExtensions();

void CleanupExtensions();

#endif
