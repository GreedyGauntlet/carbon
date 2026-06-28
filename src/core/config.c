#include "config.h"
#include "util/logger.h"

static AppConfig g_config = { 0 };
static AppConfig g_default_config = { 
    2.0f,
    1,
    (Camera2D){ (Vector2){ 0, 0}, (Vector2){ 0, 0 }, 0, 1.0f },
    FALSE,
    LEVEL_NONE,
    TRUE,
    TRUE,
    LEVEL_NONE,
    TRUE,
    TRUE,
    FALSE
};

AppConfig* Config() {
    return &g_config;
}

void ResetConfig() {
    g_config = g_default_config;
}

void InitConfig() {
    ResetConfig();
    FILE *file = fopen(".carbonconf", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        if (filesize == sizeof(AppConfig)) {
            rewind(file);
            fread(Config(), 1, sizeof(AppConfig), file);
            logtrace("Successfully loaded existing config");
        } else {
            logwarn("Existing config is invalid - falling back to default config");
        }
        fclose(file);
    } else {
        logtrace("Unable to detect existing config - using default config");
    }
}

void CleanConfig() {
    FILE *file = fopen(".carbonconf", "wb");
    if (file) {
        fwrite(Config(), 1, sizeof(AppConfig), file);
        fclose(file);
    }
}
