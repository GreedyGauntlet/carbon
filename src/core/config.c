#include "config.h"

static AppConfig g_config = { 2.0f, 1 };

AppConfig* Config() {
    return &g_config;
}
