#include "keymap.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/types/hashtable.h"

static Hashtable *keymap = NULL;

#define KEYMAP_SET(key, value) \
	hashtable_set(keymap, HASH_KEY(str, key), HASH_VAL(i32, value))

void keymap_init() {
	keymap = hashtable_create(HASHTABLE_KEY_STRING, 256, false);

	KEYMAP_SET("Escape", LS_KEY_ESCAPE);
	KEYMAP_SET("F1", LS_KEY_F1);
	KEYMAP_SET("F2", LS_KEY_F2);
	KEYMAP_SET("F3", LS_KEY_F3);
	KEYMAP_SET("F4", LS_KEY_F4);
	KEYMAP_SET("F5", LS_KEY_F5);
	KEYMAP_SET("F6", LS_KEY_F6);
	KEYMAP_SET("F7", LS_KEY_F7);
	KEYMAP_SET("F8", LS_KEY_F8);
	KEYMAP_SET("F9", LS_KEY_F9);
	KEYMAP_SET("F10", LS_KEY_F10);
	KEYMAP_SET("F11", LS_KEY_F11);
	KEYMAP_SET("F12", LS_KEY_F12);
	KEYMAP_SET("PrintScreen", LS_KEY_PRINT_SCREEN);
	KEYMAP_SET("ScrollLock", LS_KEY_SCROLL_LOCK);
	KEYMAP_SET("Pause", LS_KEY_PAUSE);
	KEYMAP_SET("Backquote", LS_KEY_GRAVE_ACCENT);
	KEYMAP_SET("Digit1", LS_KEY_1);
	KEYMAP_SET("Digit2", LS_KEY_2);
	KEYMAP_SET("Digit3", LS_KEY_3);
	KEYMAP_SET("Digit4", LS_KEY_4);
	KEYMAP_SET("Digit5", LS_KEY_5);
	KEYMAP_SET("Digit6", LS_KEY_6);
	KEYMAP_SET("Digit7", LS_KEY_7);
	KEYMAP_SET("Digit8", LS_KEY_8);
	KEYMAP_SET("Digit9", LS_KEY_9);
	KEYMAP_SET("Digit0", LS_KEY_0);
	KEYMAP_SET("Minus", LS_KEY_MINUS);
	KEYMAP_SET("Equal", LS_KEY_EQUAL);
	KEYMAP_SET("Backspace", LS_KEY_BACKSPACE);
	KEYMAP_SET("Tab", LS_KEY_TAB);
	KEYMAP_SET("KeyQ", LS_KEY_Q);
	KEYMAP_SET("KeyW", LS_KEY_W);
	KEYMAP_SET("KeyE", LS_KEY_E);
	KEYMAP_SET("KeyR", LS_KEY_R);
	KEYMAP_SET("KeyT", LS_KEY_T);
	KEYMAP_SET("KeyY", LS_KEY_Y);
	KEYMAP_SET("KeyU", LS_KEY_U);
	KEYMAP_SET("KeyI", LS_KEY_I);
	KEYMAP_SET("KeyO", LS_KEY_O);
	KEYMAP_SET("KeyP", LS_KEY_P);
	KEYMAP_SET("BracketLeft", LS_KEY_LEFT_BRACKET);
	KEYMAP_SET("BracketRight", LS_KEY_RIGHT_BRACKET);
	KEYMAP_SET("Backslash", LS_KEY_BACKSLASH);
	KEYMAP_SET("CapsLock", LS_KEY_CAPS_LOCK);
	KEYMAP_SET("KeyA", LS_KEY_A);
	KEYMAP_SET("KeyS", LS_KEY_S);
	KEYMAP_SET("KeyD", LS_KEY_D);
	KEYMAP_SET("KeyF", LS_KEY_F);
	KEYMAP_SET("KeyG", LS_KEY_G);
	KEYMAP_SET("KeyH", LS_KEY_H);
	KEYMAP_SET("KeyJ", LS_KEY_J);
	KEYMAP_SET("KeyK", LS_KEY_K);
	KEYMAP_SET("KeyL", LS_KEY_L);
	KEYMAP_SET("Semicolon", LS_KEY_SEMICOLON);
	KEYMAP_SET("Quote", LS_KEY_APOSTROPHE);
	KEYMAP_SET("Enter", LS_KEY_ENTER);
	KEYMAP_SET("ShiftLeft", LS_KEY_LEFT_SHIFT);
	KEYMAP_SET("KeyZ", LS_KEY_Z);
	KEYMAP_SET("KeyX", LS_KEY_X);
	KEYMAP_SET("KeyC", LS_KEY_C);
	KEYMAP_SET("KeyV", LS_KEY_V);
	KEYMAP_SET("KeyB", LS_KEY_B);
	KEYMAP_SET("KeyN", LS_KEY_N);
	KEYMAP_SET("KeyM", LS_KEY_M);
	KEYMAP_SET("Comma", LS_KEY_COMMA);
	KEYMAP_SET("Period", LS_KEY_PERIOD);
	KEYMAP_SET("Slash", LS_KEY_SLASH);
	KEYMAP_SET("ShiftRight", LS_KEY_RIGHT_SHIFT);
	KEYMAP_SET("ControlLeft", LS_KEY_LEFT_CONTROL);
	KEYMAP_SET("MetaLeft", LS_KEY_LEFT_SUPER);
	KEYMAP_SET("AltLeft", LS_KEY_LEFT_ALT);
	KEYMAP_SET("Space", LS_KEY_SPACE);
	KEYMAP_SET("AltRight", LS_KEY_RIGHT_ALT);
	KEYMAP_SET("MetaRight", LS_KEY_RIGHT_SUPER);
	KEYMAP_SET("ContextMenu", LS_KEY_MENU);
	KEYMAP_SET("ControlRight", LS_KEY_RIGHT_CONTROL);
	KEYMAP_SET("ArrowLeft", LS_KEY_LEFT);
	KEYMAP_SET("ArrowUp", LS_KEY_UP);
	KEYMAP_SET("ArrowRight", LS_KEY_RIGHT);
	KEYMAP_SET("ArrowDown", LS_KEY_DOWN);
	KEYMAP_SET("Insert", LS_KEY_INSERT);
	KEYMAP_SET("Delete", LS_KEY_DELETE);
	KEYMAP_SET("Home", LS_KEY_HOME);
	KEYMAP_SET("End", LS_KEY_END);
	KEYMAP_SET("PageUp", LS_KEY_PAGE_UP);
	KEYMAP_SET("PageDown", LS_KEY_PAGE_DOWN);
	KEYMAP_SET("NumLock", LS_KEY_NUM_LOCK);
	KEYMAP_SET("NumpadDivide", LS_KEY_KP_DIVIDE);
	KEYMAP_SET("NumpadMultiply", LS_KEY_KP_MULTIPLY);
	KEYMAP_SET("NumpadSubtract", LS_KEY_KP_SUBTRACT);
	KEYMAP_SET("NumpadAdd", LS_KEY_KP_ADD);
	KEYMAP_SET("NumpadEnter", LS_KEY_KP_ENTER);
	KEYMAP_SET("NumpadDecimal", LS_KEY_KP_DECIMAL);
	KEYMAP_SET("Numpad0", LS_KEY_KP_0);
	KEYMAP_SET("Numpad1", LS_KEY_KP_1);
	KEYMAP_SET("Numpad2", LS_KEY_KP_2);
	KEYMAP_SET("Numpad3", LS_KEY_KP_3);
	KEYMAP_SET("Numpad4", LS_KEY_KP_4);
	KEYMAP_SET("Numpad5", LS_KEY_KP_5);
	KEYMAP_SET("Numpad6", LS_KEY_KP_6);
	KEYMAP_SET("Numpad7", LS_KEY_KP_7);
	KEYMAP_SET("Numpad8", LS_KEY_KP_8);
	KEYMAP_SET("Numpad9", LS_KEY_KP_9);
}

void keymap_deinit() {
	hashtable_destroy(keymap);
	keymap = NULL;
}

LSKeycode web_map_key(String key) {
	LS_ASSERT(keymap);
	const HashtableKey hash_key = HASH_KEY(str, key);

	if (!hashtable_contains(keymap, hash_key)) {
		return LS_KEY_UNKNOWN;
	}

	return hashtable_get(keymap, hash_key).i32;
}