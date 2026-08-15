#include "extra.h"

BOOL ParseMessageLevel(const char* str, MessageLevel* value) {
    if (strcmp(str, "NONE") == 0) {
        *value = LEVEL_NONE;
    } else if (strcmp(str, "TRACE") == 0) {
        *value = LEVEL_TRACE;
    } else if (strcmp(str, "INFO") == 0) {
        *value = LEVEL_INFO;
    } else if (strcmp(str, "WARN") == 0) {
        *value = LEVEL_WARN;
    } else if (strcmp(str, "ERROR") == 0) {
        *value = LEVEL_ERROR;
    } else {
        return FALSE;
    }
    return TRUE;
}
