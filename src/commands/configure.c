#include "configure.h"
#include "util/logger.h"
#include "util/parse.h"
#include "core/config.h"

#define REGISTER_CONFIG_READ(name, fmt) if (strcmp(#name, argv[1]) == 0) { loginfo(fmt, Config()->name); return TRUE; }
#define REGISTER_CONFIG_READ_BOOL(name) if (strcmp(#name, argv[1]) == 0) { loginfo("%s", Config()->name ? "true" : "false"); return TRUE; }
#define REGISTER_CONFIG_READ_MESSAGELEVEL(name) if (strcmp(#name, argv[1]) == 0) { loginfo("%s", \
    (Config()->name  == LEVEL_NONE ? "NONE" : \
        (Config()->name == LEVEL_TRACE ? "TRACE" : \
            (Config()->name == LEVEL_INFO ? "INFO" : \
                (Config()->name == LEVEL_WARN ? "WARN" : "ERROR"))))); return TRUE; }
#define REGISTER_CONFIG_WRITE(name, type) if (strcmp(#name, argv[1]) == 0) { \
    if (Parse##type(argv[2], &(Config()->name))) {  \
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
        REGISTER_CONFIG_READ(ffspeed, "%.3f");
        REGISTER_CONFIG_READ(stepsize, "%lu");
        REGISTER_CONFIG_READ_BOOL(printlogs);
        REGISTER_CONFIG_READ_BOOL(echologs);
        REGISTER_CONFIG_READ_MESSAGELEVEL(notificationfilter);
        REGISTER_CONFIG_READ_BOOL(enablenotifications);
        REGISTER_CONFIG_READ_BOOL(logsnotify);
        REGISTER_CONFIG_READ_BOOL(flipnotifications);
        REGISTER_CONFIG_READ_BOOL(startupscene);
        REGISTER_CONFIG_READ_BOOL(vsync);
        logerror("Unknown config parameter \"%s\" detected - unable to read", argv[1]);
    } else if (strcmp(argv[0], "write") == 0) {
        if (argc != 3) {
            logerror("Exactly 3 arguments expected - ocnfig write <parameter> <value>");
            return FALSE;
        }
        REGISTER_CONFIG_WRITE(ffspeed, Float);
        REGISTER_CONFIG_WRITE(stepsize, Size);
        REGISTER_CONFIG_WRITE(printlogs, Bool);
        REGISTER_CONFIG_WRITE(echologs, Bool);
        REGISTER_CONFIG_WRITE(notificationfilter, Filter);
        REGISTER_CONFIG_WRITE(enablenotifications, Bool);
        REGISTER_CONFIG_WRITE(logsnotify, Bool);
        REGISTER_CONFIG_WRITE(flipnotifications, Bool);
        REGISTER_CONFIG_WRITE(startupscene, Bool);
        REGISTER_CONFIG_WRITE(vsync, Bool);
        logerror("Unable to write to unknown config parameter \"%s\"", argv[1]);
    } else {
        logerror("Unknown second argument \"%s\" detected - expected <read/write>", argv[0]);
        return FALSE;
    }
    return TRUE;
}
