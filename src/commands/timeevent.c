#include "timeevent.h"
#include "ui/panels/console.h"
#include "core/entrypoint.h"
#include "util/logger.h"
#include "util/parse.h"
#include <easyobjects.h>
#include <easybasics.h>

DECLARE_ARRLIST_NAMED(charPtr, char*);
IMPL_ARRLIST_NAMED(charPtr, char*);

static ARRLIST_charPtr g_eventcommands = { 0 };
static ARRLIST_float g_eventtimes = { 0 };

static void UpdateTimeEvents() {
    for (size_t i = 0; i < g_eventcommands.size; i++) {
        g_eventtimes.data[i] -= GetFrameTime();
        if (g_eventtimes.data[i] <= 0.0f) {
            ExecuteCommand(g_eventcommands.data[i]);
            EZ_FREE(g_eventcommands.data[i]);
            ARRLIST_charPtr_remove(&g_eventcommands, i);
            ARRLIST_float_remove(&g_eventtimes, i);
            i--;
        }
    }
}

static void CleanTimeEvents() {
    for (size_t i = 0; i < g_eventcommands.size; i++) EZ_FREE(g_eventcommands.data[i]);
    ARRLIST_charPtr_clear(&g_eventcommands);
    ARRLIST_float_clear(&g_eventtimes);
}

BOOL TimeEventCommand(char** argv, int argc) {
    if (argc < 2) {
        logerror("Not enough arguments specified - there must be at least 1 float and 1 string command!");
        return FALSE;
    }
    float time;
    if (!ParseFloat(argv[0], &time)) {
        logerror("Unable to parse \"%s\" as a float", argv[0]);
        return FALSE;
    }
    ARRLIST_charPtr_add(&g_eventcommands, ReconstructCommand(&(argv[1]), argc - 1));
    ARRLIST_float_add(&g_eventtimes, time);
    logtrace("Registered command \"%s\" to run in %.3f seconds", g_eventcommands.data[g_eventcommands.size - 1], time);
    return TRUE;
}

REGISTER_PREUPDATE(UpdateTimeEvents);
REGISTER_CLEANUP(CleanTimeEvents);
