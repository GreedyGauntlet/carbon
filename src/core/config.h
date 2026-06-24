#ifndef CONFIG_H
#define CONFIG_H

#include "data/declarations.h"

typedef struct {
    float ffspeed;
    size_t stepsize;
    Camera2D camera;
    BOOL limitlogs;
    MessageLevel logfilter;
} AppConfig;

AppConfig* Config();

#endif
