#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "data/declarations.h"

void DrawNotifications();

void CleanNotifications();

void Notify(MessageLevel level, char* message, ...);

#endif
