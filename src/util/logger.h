#ifndef LOGGER_H
#define LOGGER_H

#include "ui/panels/console.h"

#define logtrace(...) SubmitConsoleOutput(LEVEL_TRACE, __VA_ARGS__)
#define loginfo(...) SubmitConsoleOutput(LEVEL_INFO, __VA_ARGS__)
#define logwarn(...) SubmitConsoleOutput(LEVEL_WARN, __VA_ARGS__)
#define logerror(...) SubmitConsoleOutput(LEVEL_ERROR, __VA_ARGS__)

#endif
