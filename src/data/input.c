#include "input.h"
#include "ui/panels/viewport.h"
#include <easylogger.h>
#include <raymath.h>

static InputMap g_input_map = { 0 };

START_INPUT_KEY_DEFINITIONS()
DEFINE_INPUT_KEY(IK_DEV, KEY_D, "D");
DEFINE_INPUT_KEY(IK_ENTER, KEY_ENTER, "Enter");
DEFINE_INPUT_KEY(IK_BACKSPACE, KEY_BACKSPACE, "Backspace");
DEFINE_INPUT_KEY(IK_LEFT, KEY_LEFT, "Left");
DEFINE_INPUT_KEY(IK_RIGHT, KEY_RIGHT, "Right");
DEFINE_INPUT_KEY(IK_UP, KEY_UP, "Up");
DEFINE_INPUT_KEY(IK_DOWN, KEY_DOWN, "Down");
DEFINE_INPUT_KEY(IK_ZOOM, KEY_Z, "Z");
DEFINE_INPUT_KEY(IK_TOGGLE_HINTS, KEY_H, "H");
DEFINE_INPUT_KEY(IK_RESET_CAMERA, KEY_GRAVE, "`");
END_INPUT_KEY_DEFINITIONS()

START_INPUT_BUTTON_DEFINITIONS()
DEFINE_INPUT_BUTTON(IK_MOUSELEFT, MOUSE_BUTTON_LEFT, "Mouse Left");
DEFINE_INPUT_BUTTON(IK_MOUSERIGHT, MOUSE_BUTTON_RIGHT, "Mouse Right");
END_INPUT_BUTTON_DEFINITIONS()

void InitializeInput() {
    InitializeKeys();
    InitializeButtons();
    g_input_map.initialized = TRUE;
}

void BlockInputs() {
    g_input_map.blocked = TRUE;
}

void UnblockInputs() {
    g_input_map.blocked = FALSE;
}

const char* InputKeyRepresentation(const int key) {
    EZ_ASSERT(key < NUM_KEY_INPUTS, "Invalid key code");
    return g_input_map.keynames[key];
}

const char* InputButtonRepresentation(const int button) {
    EZ_ASSERT(button < NUM_BUTTON_INPUTS, "Invalid button code");
    return g_input_map.btnnames[button];
}

BOOL InputKeyPressed(const int key) {
    EZ_ASSERT(key < NUM_KEY_INPUTS, "Invalid key code");
    if (g_input_map.blocked) return FALSE;
    return IsKeyPressed(g_input_map.keymap[key]);
}

BOOL InputKeyReleased(const int key) {
    EZ_ASSERT(key < NUM_KEY_INPUTS, "Invalid key code");
    if (g_input_map.blocked) return FALSE;
    return IsKeyReleased(g_input_map.keymap[key]);
}

BOOL InputKeyDown(const int key) {
    EZ_ASSERT(key < NUM_KEY_INPUTS, "Invalid key code");
    if (g_input_map.blocked) return FALSE;
    return IsKeyDown(g_input_map.keymap[key]);
}

BOOL InputKeyUp(const int key) {
    EZ_ASSERT(key < NUM_KEY_INPUTS, "Invalid key code");
    if (g_input_map.blocked) return FALSE;
    return IsKeyUp(g_input_map.keymap[key]);
}

BOOL InputButtonPressed(const int btn) {
    EZ_ASSERT(btn < NUM_BUTTON_INPUTS, "Invalid button code");
    if (g_input_map.blocked) return FALSE;
    return IsMouseButtonPressed(g_input_map.btnmap[btn]);
}

BOOL InputButtonReleased(const int btn) {
    EZ_ASSERT(btn < NUM_BUTTON_INPUTS, "Invalid button code");
    if (g_input_map.blocked) return FALSE;
    return IsMouseButtonReleased(g_input_map.btnmap[btn]);
}

BOOL InputButtonDown(const int btn) {
    EZ_ASSERT(btn < NUM_BUTTON_INPUTS, "Invalid button code");
    if (g_input_map.blocked) return FALSE;
    return IsMouseButtonDown(g_input_map.btnmap[btn]);
}

BOOL InputButtonUp(const int btn) {
    EZ_ASSERT(btn < NUM_BUTTON_INPUTS, "Invalid button code");
    if (g_input_map.blocked) return FALSE;
    return IsMouseButtonUp(g_input_map.btnmap[btn]);
}

Vector2 InputMousePosition() {
    return Vector2Subtract(GetMousePosition(), GetViewportPosition());
}
