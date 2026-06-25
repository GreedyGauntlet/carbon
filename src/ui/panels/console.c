#include "console.h"
#include "data/definitions.h"
#include "data/colors.h"
#include "data/input.h"
#include "data/fonts.h"
#include "core/config.h"
#include "ui/ui.h"

#define MAX_COMMAND_SIZE 256
#define MAX_OUTPUT_SIZE (512 + MAX_COMMAND_SIZE)
#define CONSOLE_HISTORY 100

static char g_commandbuffer[MAX_COMMAND_SIZE] = { 0 };
static char g_outputbuffer[CONSOLE_HISTORY][MAX_OUTPUT_SIZE] = { 0 };
static MessageLevel g_loglevels[CONSOLE_HISTORY] = { 0 };
static size_t g_history_pointer = 0;
static char* g_log_filter_labels[5] = { "none", "traces", "info", "warnings", "errors" };
static float g_scrolldiff = 0.0f;

static size_t DropdownSelectLogFilter(void* data, size_t index) {
    if (index != (size_t)-1) {
        Config()->logfilter = (MessageLevel)index;
    } else {
        return (size_t)Config()->logfilter;
    }
    return index;
}

static int Milli(float time) {
    return ((int)(time * 100)) % 100;
}

static int Secs(float time) {
    return ((int)time) % 60;
}

static int Mins(float time) {
    return ((int)time / 60) % 60;
}

static int Hours(float time) {
    return ((int)time / 3600);
}

static void SubmitConsoleOutput(const char* output, MessageLevel level) {
    if (g_history_pointer == 0) g_history_pointer = CONSOLE_HISTORY - 1;
    else g_history_pointer--;
    float time = GetTime();
    snprintf(g_outputbuffer[g_history_pointer], MAX_OUTPUT_SIZE - 1, "[%s%d:%s%d:%s%d.%d] %s ",
             Hours(time) < 10 ? "0" : "", Hours(time),
             Mins(time) < 10 ? "0" : "", Mins(time),
             Secs(time) < 10 ? "0" : "", Secs(time),
             Milli(time), output);
    g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 1] = '\0';
    if (strnlen(output, MAX_OUTPUT_SIZE) > MAX_OUTPUT_SIZE - 14) {
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 2] = ' ';
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 3] = ')';
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 4] = '.';
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 5] = '.';
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 6] = '.';
        g_outputbuffer[g_history_pointer][MAX_OUTPUT_SIZE - 7] = '(';
    }
    g_loglevels[g_history_pointer] = level;
}

static float CalculateLogHeight(float width) {
    float height = 0.0f;
    size_t index = g_history_pointer;
    do {
        if (g_outputbuffer[index][0] == '\0') break;
        float fullwidth = UITextWidth(g_outputbuffer[index]);
        height += (((int)(fullwidth / width)) + (fullwidth == width ? 0 : 1)) * LINE_HEIGHT;
        index = (index + 1) % CONSOLE_HISTORY;
    } while (index != g_history_pointer);
    return height;
}

static BOOL DrawLog(float* cursory, float width, size_t* index) {
    if (g_outputbuffer[*index][0] == '\0') return FALSE;
    if (Config()->logfilter != LEVEL_NONE) {
        BOOL yes = Config()->logfilter == g_loglevels[*index];
        if (Config()->limitlogs && g_loglevels[*index] > Config()->logfilter) yes = TRUE;
        if (!yes) {
            *index = (*index + 1) % CONSOLE_HISTORY;
            if (*index == g_history_pointer || g_outputbuffer[*index][0] == '\0') return FALSE;
            return TRUE;
        }
    }
    char* output = g_outputbuffer[*index];
    float fullwidth = UITextWidth(output);
    size_t numlines = ((int)(fullwidth / width)) + (fullwidth == width ? 0 : 1);
    *cursory -= numlines * LINE_HEIGHT;
    char* phrase = output;
    char buffer[MAX_OUTPUT_SIZE] = { 0 };
    int i = 0;
    char* oldp = phrase;
    size_t oldi = i;
    BOOL first = TRUE;
    Color logcolor = PURPLE;
    switch (g_loglevels[*index]) {
        case LEVEL_TRACE:
            logcolor = MappedColor(LOG_TRACE_COLOR);
            break;
        case LEVEL_INFO:
            logcolor = MappedColor(LOG_INFO_COLOR);
            break;
        case LEVEL_WARN:
            logcolor = MappedColor(LOG_WARN_COLOR);
            break;
        case LEVEL_ERROR:
            logcolor = MappedColor(LOG_ERROR_COLOR);
            break;
        default: break;
    }
    while (phrase[0] != '\0') {
        buffer[i] = phrase[0];
        if (buffer[i] == ' ') {
            float twidth = UITextWidth(buffer);
            if (twidth > width) {
                if (first) {
                    oldi = i;
                    oldp = phrase;
                }
                buffer[oldi] = '\0';
                DrawTextEx(FontAsset(), buffer, (Vector2){ 10, *cursory }, LINE_HEIGHT, 0, logcolor);
                *cursory += LINE_HEIGHT;
                memset(buffer, '\0', MAX_OUTPUT_SIZE);
                i = -1;
                phrase = oldp;
                first = TRUE;
            } else {
                oldp = phrase;
                oldi = i;
                first = FALSE;
            }
        }
        phrase++;
        i++;
    }
    DrawTextEx(FontAsset(), buffer, (Vector2){ 10, *cursory }, LINE_HEIGHT, 0, logcolor);
    *cursory -= (numlines - 1) * LINE_HEIGHT;
    if (*cursory < 0 - LINE_HEIGHT) return FALSE;
    *index = (*index + 1) % CONSOLE_HISTORY;
    if (*index == g_history_pointer || g_outputbuffer[*index][0] == '\0') return FALSE;
    return TRUE;
}

static void DrawConsolePanel(float width, float height) {
    float logheight = CalculateLogHeight(width - 10);
    if (HoveredPanel() && strcmp(HoveredPanel(), "Console") == 0) {
        if (InputKeyPressed(IK_ENTER)) g_scrolldiff = 0.0f;
        g_scrolldiff += 18.0f * GetMouseWheelMove();
    }
    if (g_scrolldiff > logheight - height + 105) g_scrolldiff = logheight - height + 105;
    if (g_scrolldiff < 0.0f) g_scrolldiff = 0.0f;
    size_t baselog = g_scrolldiff / LINE_HEIGHT;
    DrawRectangle(5, 30, width - 10, height - 100, MappedColor(PANEL_GRAPH_BG_COLOR));
    size_t i = (g_history_pointer + baselog) % CONSOLE_HISTORY;
    float starty = height - 75 - (baselog * LINE_HEIGHT) + g_scrolldiff;
    while (DrawLog(&starty, width - 10, &i)) {}
    DrawRectangle(0, 0, width, 30, MappedColor(PANEL_BG_COLOR));
    DrawRectangle(width - 5, 0, 5, height, MappedColor(PANEL_BG_COLOR));
    UISetCursor(width - UITextWidth("Limit:") - 30, 5);
    UICheckboxLabeled("Limit:", &(Config()->limitlogs));
    UISetCursor(5, 5);
    UIDrawText("Filter By");
    UIMoveCursor(UITextWidth("Filter By") + 5, -20);
    float ddw = width - UITextWidth("Filter By") - UITextWidth("Limit:") - 80;
    if (ddw > 200) ddw = 200;
    UIDropdownMenu(ddw, 5, g_log_filter_labels, DropdownSelectLogFilter, NULL);
    DrawRectangle(0, height - 70, width, 20, MappedColor(PANEL_BG_COLOR));
    UISetCursor(10, height - 75);
    UIDivider(width - 20);
    if (UITextInput("cmd >>", g_commandbuffer, sizeof(g_commandbuffer), width - 20, TRUE)) {
        SubmitConsoleOutput(g_commandbuffer, LEVEL_TRACE);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_INFO);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_WARN);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_ERROR);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_TRACE);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_INFO);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_WARN);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_ERROR);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_TRACE);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_INFO);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_WARN);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_ERROR);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_TRACE);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_INFO);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_WARN);
        SubmitConsoleOutput(g_commandbuffer, LEVEL_ERROR);
        memset(g_commandbuffer, '\0', MAX_COMMAND_SIZE);
    }
}

Panel GenerateConsolePanel() {
	Panel p = { 0 };
	SetupPanel(&p, "Console");
	p.draw = DrawConsolePanel;
	return p;
}

// TODO:
// - command sustem
