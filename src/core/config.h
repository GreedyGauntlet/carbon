#ifndef CONFIG_H
#define CONFIG_H

#include <raylib.h>
#include <stddef.h>

typedef struct {
    float ffspeed;
    size_t stepsize;
    Camera2D camera;
} AppConfig;

AppConfig* Config();

#endif
