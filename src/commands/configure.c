#include "configure.h"
#include <util/logger.h>
#include <core/config.h>
#include <easyparse.h>

#define REGISTER_CONFIG_READ_FLOAT(name, fmt) if (strcmp(#name, argv[1]) == 0) { loginfo(fmt, ConfigGetFloat(#name)); return TRUE; }
#define REGISTER_CONFIG_READ_SIZE(name, fmt) if (strcmp(#name, argv[1]) == 0) { loginfo(fmt, ConfigGetSize(#name)); return TRUE; }
#define REGISTER_CONFIG_READ_BOOL(name) if (strcmp(#name, argv[1]) == 0) { loginfo("%s", ConfigGetBool(#name) ? "true" : "false"); return TRUE; }
#define REGISTER_CONFIG_READ_MESSAGELEVEL(name) if (strcmp(#name, argv[1]) == 0) { \
    MessageLevel ml = ConfigGetMessageLevel(#name); \
    loginfo("%s", \
    (ml == LEVEL_NONE ? "NONE" : \
        (ml == LEVEL_TRACE ? "TRACE" : \
            (ml == LEVEL_INFO ? "INFO" : \
                (ml == LEVEL_WARN ? "WARN" : "ERROR"))))); return TRUE; }
#define REGISTER_CONFIG_WRITE_FLOAT(name) if (strcmp(#name, argv[1]) == 0) { \
    float wv = ConfigGetFloat(#name); \
    if (ez_parse_float(argv[2], &wv)) {  \
        ConfigSetFloat(#name, wv); \
        logtrace("Successfully set \"%s\" to \"%s\"", #name, argv[2]); \
        return TRUE; \
    } else { \
        logerror("Unable to parse \"%s\"", argv[2]); \
        return FALSE; \
    } \
}
#define REGISTER_CONFIG_WRITE_SIZE(name) if (strcmp(#name, argv[1]) == 0) { \
    size_t wv = ConfigGetSize(#name); \
    if (ez_parse_size(argv[2], &wv)) {  \
        ConfigSetSize(#name, wv); \
        logtrace("Successfully set \"%s\" to \"%s\"", #name, argv[2]); \
        return TRUE; \
    } else { \
        logerror("Unable to parse \"%s\"", argv[2]); \
        return FALSE; \
    } \
}
#define REGISTER_CONFIG_WRITE_BOOL(name) if (strcmp(#name, argv[1]) == 0) { \
    BOOL wv = ConfigGetBool(#name); \
    if (ez_parse_bool(argv[2], &wv)) {  \
        ConfigSetBool(#name, wv); \
        logtrace("Successfully set \"%s\" to \"%s\"", #name, argv[2]); \
        return TRUE; \
    } else { \
        logerror("Unable to parse \"%s\"", argv[2]); \
        return FALSE; \
    } \
}
#define REGISTER_CONFIG_WRITE_MESSAGELEVEL(name) if (strcmp(#name, argv[1]) == 0) { \
    MessageLevel wv = ConfigGetMessageLevel(#name); \
    if (ParseMessageLevel(argv[2], &wv)) {  \
        ConfigSetMessageLevel(#name, wv); \
        logtrace("Successfully set \"%s\" to \"%s\"", #name, argv[2]); \
        return TRUE; \
    } else { \
        logerror("Unable to parse \"%s\"", argv[2]); \
        return FALSE; \
    } \
}

BOOL ConfigureCommand(char** argv, int argc) {
    if (argc < 2) {
        logerror("At least 2 arguments expected - config <read/write> <...>");
        return FALSE;
    }
    if (strcmp(argv[0], "read") == 0) {
        if (argc != 2) {
            logerror("Exactly 2 arguments expected - ocnfig read <parameter>");
            return FALSE;
        }
        REGISTER_CONFIG_READ_FLOAT(ffspeed, "%.3f");
        REGISTER_CONFIG_READ_SIZE(stepsize, "%lu");
        REGISTER_CONFIG_READ_BOOL(printlogs);
        REGISTER_CONFIG_READ_BOOL(echologs);
        REGISTER_CONFIG_READ_MESSAGELEVEL(notificationfilter);
        REGISTER_CONFIG_READ_BOOL(enablenotifications);
        REGISTER_CONFIG_READ_BOOL(logsnotify);
        REGISTER_CONFIG_READ_BOOL(flipnotifications);
        REGISTER_CONFIG_READ_BOOL(startupscene);
        REGISTER_CONFIG_READ_BOOL(vsync);
        REGISTER_CONFIG_READ_BOOL(startupentity);
        REGISTER_CONFIG_READ_BOOL(enableclickselection);
        logerror("Unknown config parameter \"%s\" detected - unable to read", argv[1]);
    } else if (strcmp(argv[0], "write") == 0) {
        if (argc != 3) {
            logerror("Exactly 3 arguments expected - ocnfig write <parameter> <value>");
            return FALSE;
        }
        REGISTER_CONFIG_WRITE_FLOAT(ffspeed);
        REGISTER_CONFIG_WRITE_SIZE(stepsize);
        REGISTER_CONFIG_WRITE_BOOL(printlogs);
        REGISTER_CONFIG_WRITE_BOOL(echologs);
        REGISTER_CONFIG_WRITE_MESSAGELEVEL(notificationfilter);
        REGISTER_CONFIG_WRITE_BOOL(enablenotifications);
        REGISTER_CONFIG_WRITE_BOOL(logsnotify);
        REGISTER_CONFIG_WRITE_BOOL(flipnotifications);
        REGISTER_CONFIG_WRITE_BOOL(startupscene);
        REGISTER_CONFIG_WRITE_BOOL(vsync);
        REGISTER_CONFIG_WRITE_BOOL(startupentity);
        REGISTER_CONFIG_WRITE_BOOL(enableclickselection);
        logerror("Unable to write to unknown config parameter \"%s\"", argv[1]);
    } else {
        logerror("Unknown second argument \"%s\" detected - expected <read/write>", argv[0]);
        return FALSE;
    }
    return TRUE;
}
