#ifndef CONSOLE_H
#define CONSOLE_H

#include "data/declarations.h"

typedef BOOL (*CommandFunction)(char** arguments, int argc);

typedef struct {
    const char* phrase;
    CommandFunction function;
} Command;

void RegisterCommand(Command command);

void SubmitConsoleOutput(MessageLevel level, const char* output, ...);

Panel GenerateConsolePanel();

#endif
