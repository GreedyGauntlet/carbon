#ifndef CONSOLE_H
#define CONSOLE_H

#include "data/declarations.h"

typedef BOOL (*CommandFunction)(char** arguments, int argc);

typedef struct {
    const char* phrase;
    CommandFunction function;
    const char* usage;
    const char* description;
} Command;

DECLARE_ARRLIST(Command);

ARRLIST_Command GetCommands();

void CleanConsoleLogs();

void RegisterCommand(Command command);

void SubmitConsoleOutput(MessageLevel level, const char* output, ...);

void ExecuteCommand(char* command);

Panel GenerateConsolePanel();

#endif
