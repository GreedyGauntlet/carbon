#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

typedef struct {
    float ffspeed;
    size_t stepsize;
} AppConfig;

AppConfig* Config();

#endif
