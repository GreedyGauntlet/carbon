#ifndef PARSE_H
#define PARSE_H

#include "data/declarations.h"

BOOL ParseFloat(const char* str, float* value);

BOOL ParseSize(const char* str, size_t* value);

BOOL ParseBool(const char* str, BOOL* value);

BOOL ParseFilter(const char* str, MessageLevel* value);

char* ReconstructCommand(char** argv, int argc);

#endif
