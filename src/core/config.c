#include "config.h"
#include "data/definitions.h"
#include "util/logger.h"
#include "core/application.h"
#include "ecs/entity.h"
#include "ui/panels/edit.h"

static AppConfig g_config = { 0 };
static AppConfig g_default_config = { 
    2.0f,
    1,
    (Camera2D){ (Vector2){ 0, 0 }, (Vector2){ 0, 0 }, 0, 1.0f },
    FALSE,
    LEVEL_NONE,
    TRUE,
    TRUE,
    LEVEL_NONE,
    TRUE,
    TRUE,
    FALSE,
    "",
    TRUE,
    TRUE,
    0,
    0,
    TRUE,
    TRUE
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
            logtrace("Successfully loaded engine config");
        } else {
            logwarn("Existing config is invalid - falling back to default config");
        }
        fclose(file);
    } else {
        logtrace("Unable to detect existing config - using default config");
    }
}

void CleanConfig() {
    strncpy(g_config.activescene, GetActiveScene()->name, 256);
    g_config.selectedworld = 0;
    Entity e = SelectedEntity();
    g_config.selectedentity = e.id;
    for (size_t i = 0; i < GetActiveScene()->worlds.size; i++) {
        if (GetActiveScene()->worlds.data[i] == e.context) {
            g_config.selectedworld = i;
            break;
        }
    }
    FILE *file = fopen(".carbonconf", "wb");
    if (file) {
        fwrite(Config(), 1, sizeof(AppConfig), file);
        fclose(file);
    }
}
