#include "parse.h"
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

BOOL ParseFloat(const char* str, float* value) {
    char *end;
    float result;
    errno = 0;
    if (!str || !value) return FALSE;
    result = strtof(str, &end);
    if (end == str || errno == ERANGE) return FALSE;
    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0') return FALSE;
    *value = result;
    return TRUE;
}

BOOL ParseSize(const char* str, size_t* value) {
    char *end;
    unsigned long result;
    errno = 0;
    if (!str || !value) return FALSE;
    result = strtoul(str, &end, 10);
    if (end == str || errno == ERANGE) return FALSE;
    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0') return FALSE;
    *value = (size_t)result;
    return TRUE;
}

BOOL ParseBool(const char* str, BOOL* value) {
    if (strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 || strcmp(str, "True") == 0) {
        *value = TRUE;
    } else if (strcmp(str, "false") == 0 || strcmp(str, "FALSE") == 0 || strcmp(str, "False") == 0) {
        *value = FALSE;
    } else {
        return FALSE;
    }
    return TRUE;
}

BOOL ParseFilter(const char* str, MessageLevel* value) {
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
