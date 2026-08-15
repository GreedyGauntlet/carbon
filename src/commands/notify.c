#include "notify.h"
#include "ui/notification.h"
#include "util/logger.h"
#include "util/extra.h"
#include <easyparse.h>

BOOL NotifyCommand(char** argv, int argc) {
    if (argc < 2) {
        logerror("Not enough arguments detected. Usage - notify <level> ...");
        return FALSE;
    }
    MessageLevel ml;
    if (!ParseMessageLevel(argv[0], &ml)) {
        logerror("Unable to parse message level \"%s\"", argv[0]);
        return FALSE;
    }
    char* message = ez_reconstruct_command(&(argv[1]), argc - 1);
    Notify(ml, message);
    EZ_FREE(message);
    logtrace("Successfully triggered notification");
    return TRUE;
}
