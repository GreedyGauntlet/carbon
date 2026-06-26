#include "config.h"

static AppConfig g_config = { 
    2.0f,
    1,
    (Camera2D){ (Vector2){ 0, 0}, (Vector2){ 0, 0 }, 0, 1.0f },
    FALSE,
    LEVEL_NONE,
    TRUE,
    TRUE
};

AppConfig* Config() {
    return &g_config;
}
