#ifndef CONFIG_H
#define CONFIG_H

#include "data/declarations.h"

typedef struct {
    float ffspeed;
    size_t stepsize;
    Camera2D camera;
    BOOL limitlogs;
    MessageLevel logfilter;
    BOOL printlogs;
    BOOL echologs;
    MessageLevel notificationfilter;
    BOOL enablenotifications;
    BOOL logsnotify;
    BOOL flipnotifications;
    char activescene[512];
    BOOL startupscene;
    BOOL vsync;
    size_t selectedentity;
    size_t selectedworld;
    BOOL startupentity;
    BOOL enableclickselection;
} AppConfig;

AppConfig* Config();

void ResetConfig();

void InitConfig();

void CleanConfig();

#endif
