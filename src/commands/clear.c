#include "clear.h"
#include <util/logger.h>

BOOL ClearCommand(char** argv, int argc) {
    if (argc == 0) {
        loginfo("No data subject targeted to clear");
        return TRUE;
    }
    if (argc == 1) {
        if (strcmp(argv[0], "notifications") == 0) {
            CleanNotifications();
        } else {
            logerror("Invalid data subject \"%s\" detected", argv[0]);
            return FALSE;
        }
    } else {
        logerror("More than one data subject targeted to clear detected");
        return FALSE;
    }
    return TRUE;
}
