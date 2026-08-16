#include "refresh.h"
#include "core/application.h"
#include "core/scene.h"
#include <util/logger.h>

BOOL RefreshCommand(char** argv, int argc) {
    if (argc == 0) {
        loginfo("No data subject targeted to refresh");
        return TRUE;
    }
    if (argc == 1) {
        if (strcmp(argv[0], "assets") == 0) {
            CleanNotifications();
            if (GetActiveScene()) {
                loginfo("Refreshing assets...");
                RefreshAssets(GetActiveScene());
                loginfo("Refreshed assets");
            } else {
                logerror("No active scene loaded");
            }
        } else {
            logerror("Invalid data subject \"%s\" detected", argv[0]);
            return FALSE;
        }
    } else {
        logerror("More than one data subject targeted to refresh detected");
        return FALSE;
    }
    return TRUE;
}
