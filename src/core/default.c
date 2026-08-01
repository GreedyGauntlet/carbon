#include "default.h"
#include "commands/configure.h"
#include "commands/timeevent.h"
#include "commands/help.h"
#include "commands/notify.h"
#include "commands/clear.h"
#include "commands/refresh.h"
#include "ecs/components.h"
#include "util/logger.h"

void DefaultPreload() { }

void DefaultPostload() {
    // register config commands here
    RegisterCommand((Command){ "help", HelpCommand, "help {usage|descriptions}", "Outputs help on console commands"});
    RegisterCommand((Command){ "config", ConfigureCommand, "config {read|write} {<param>} (<value>)", "Read or write to the editor config" });
    RegisterCommand((Command){ "timeevent", TimeEventCommand, "timeevent {<seconds>} ...", "Set a command to execute in a given amount of time" });
    RegisterCommand((Command){ "notify", NotifyCommand, "notify <level> ...", "Send a notification of a given level and with the given text" });
    RegisterCommand((Command){ "clear", ClearCommand, "clear {notifications}", "Clear a given data subject" });
    RegisterCommand((Command){ "refresh", RefreshCommand, "refresh {assets}", "Refresh a given data subject" });
}

void DefaultPreupdate() { }

void DefaultCleanup() { }

REGISTER_PRELOAD(DefaultPreload);
REGISTER_POSTLOAD(DefaultPostload);
REGISTER_PREUPDATE(DefaultPreupdate);
REGISTER_CLEANUP(DefaultCleanup);
