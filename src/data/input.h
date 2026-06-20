#ifndef INPUT_H
#define INPUT_H

/*
 * NOTE: editor-exclusive input. Not to be confused with game object input.
 */

#include <easybool.h>
#include <raylib.h>

#define START_INPUT_KEY_DECLARATIONS() enum { CLEAN_INPUT_KEY_COUNTER = __COUNTER__ }; void InitializeKeys();
#define DECLARE_INPUT_KEY(inputname) enum { inputname = __COUNTER__ - CLEAN_INPUT_KEY_COUNTER - 1 }
#define END_INPUT_KEY_DECLARATIONS() enum { NUM_KEY_INPUTS = __COUNTER__ - CLEAN_INPUT_KEY_COUNTER - 1 }
#define START_INPUT_KEY_DEFINITIONS() void InitializeKeys() {
#define END_INPUT_KEY_DEFINITIONS() }
#define DEFINE_INPUT_KEY(inputname, keybind, name) g_input_map.keymap[inputname] = keybind; g_input_map.keynames[inputname] = name;

#define START_INPUT_BUTTON_DECLARATIONS() enum { CLEAN_INPUT_BUTTON_COUNTER = __COUNTER__ }; void InitializeButtons();
#define DECLARE_INPUT_BUTTON(inputname) enum { inputname = __COUNTER__ - CLEAN_INPUT_BUTTON_COUNTER - 1 }
#define END_INPUT_BUTTON_DECLARATIONS() enum { NUM_BUTTON_INPUTS = __COUNTER__ - CLEAN_INPUT_BUTTON_COUNTER - 1 }
#define START_INPUT_BUTTON_DEFINITIONS() void InitializeButtons() {
#define END_INPUT_BUTTON_DEFINITIONS() }
#define DEFINE_INPUT_BUTTON(inputname, buttonbind, name) g_input_map.btnmap[inputname] = buttonbind; g_input_map.btnnames[inputname] = name;

START_INPUT_KEY_DECLARATIONS();
DECLARE_INPUT_KEY(IK_DEV);
DECLARE_INPUT_KEY(IK_ENTER);
DECLARE_INPUT_KEY(IK_BACKSPACE);
DECLARE_INPUT_KEY(IK_LEFT);
DECLARE_INPUT_KEY(IK_RIGHT);
DECLARE_INPUT_KEY(IK_UP);
DECLARE_INPUT_KEY(IK_DOWN);
END_INPUT_KEY_DECLARATIONS();

START_INPUT_BUTTON_DECLARATIONS();
DECLARE_INPUT_BUTTON(IK_MOUSELEFT);
DECLARE_INPUT_BUTTON(IK_MOUSERIGHT);
END_INPUT_BUTTON_DECLARATIONS();

typedef struct {
    BOOL initialized;
    BOOL blocked;
    KeyboardKey keymap[NUM_KEY_INPUTS];
    MouseButton btnmap[NUM_BUTTON_INPUTS];
    const char* keynames[NUM_KEY_INPUTS];
    const char* btnnames[NUM_BUTTON_INPUTS];
} InputMap;

void InitializeInput();

void BlockInputs();

void UnblockInputs();

const char* InputKeyRepresentation(const int key);

const char* InputButtonRepresentation(const int button);

BOOL InputKeyPressed(const int key);

BOOL InputKeyReleased(const int key);

BOOL InputKeyDown(const int key);

BOOL InputKeyUp(const int key);

BOOL InputButtonPressed(const int btn);

BOOL InputButtonReleased(const int btn);

BOOL InputButtonDown(const int btn);

BOOL InputButtonUp(const int btn);

#endif
