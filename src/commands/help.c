#include "help.h"
#include "ui/panels/console.h"
#include "util/logger.h"

BOOL HelpCommand(char** argv, int argc) {
    if (argc == 0) {
        loginfo("You can use this command with either the \"usage\" or \"descriptions\" arguments to print out the respective usage or descriptions of all available commands.");
        return TRUE;
    }
    if (argc == 1) {
        if (strcmp(argv[0], "usage") == 0) {
            ARRLIST_Command commands = GetCommands();
            for (size_t i = 0; i < commands.size; i++) loginfo("%d. %s", i + 1, commands.data[i].usage);
        } else if (strcmp(argv[0], "descriptions") == 0) {
            ARRLIST_Command commands = GetCommands();
            for (size_t i = 0; i < commands.size; i++) loginfo("%d. %s : %s", i + 1, commands.data[i].phrase, commands.data[i].description);
        } else {
            logerror("Invalid usage detected. Correct usage - help <usage|descriptions>");
            return FALSE;
        }
    } else {
        logerror("Invalid usage detected. Correct usage - help <usage|descriptions>");
        return FALSE;
    }
    return TRUE;
}
