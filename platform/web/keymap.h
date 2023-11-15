#ifndef WEB_KEYMAP_H
#define WEB_KEYMAP_H

#include "core/input/keycodes.h"
#include "core/types/typedefs.h"

#include <emscripten/html5.h>

// TODO: This is not great. Should probably use a hashmap here.
_FORCE_INLINE_ LSKeycode web_map_key(String keycode) {
	if (ls_str_equals(keycode, "Escape")) {
		return LS_KEY_ESCAPE;
	} else if (ls_str_equals(keycode, "F1")) {
		return LS_KEY_F1;
	} else if (ls_str_equals(keycode, "F2")) {
		return LS_KEY_F2;
	} else if (ls_str_equals(keycode, "F3")) {
		return LS_KEY_F3;
	} else if (ls_str_equals(keycode, "F4")) {
		return LS_KEY_F4;
	} else if (ls_str_equals(keycode, "F5")) {
		return LS_KEY_F5;
	} else if (ls_str_equals(keycode, "F6")) {
		return LS_KEY_F6;
	} else if (ls_str_equals(keycode, "F7")) {
		return LS_KEY_F7;
	} else if (ls_str_equals(keycode, "F8")) {
		return LS_KEY_F8;
	} else if (ls_str_equals(keycode, "F9")) {
		return LS_KEY_F9;
	} else if (ls_str_equals(keycode, "F10")) {
		return LS_KEY_F10;
	} else if (ls_str_equals(keycode, "F11")) {
		return LS_KEY_F11;
	} else if (ls_str_equals(keycode, "F12")) {
		return LS_KEY_F12;
	} else if (ls_str_equals(keycode, "PrintScreen")) {
		return LS_KEY_PRINT_SCREEN;
	} else if (ls_str_equals(keycode, "ScrollLock")) {
		return LS_KEY_SCROLL_LOCK;
	} else if (ls_str_equals(keycode, "Pause")) {
		return LS_KEY_PAUSE;
	} else if (ls_str_equals(keycode, "Backquote")) {
		return LS_KEY_GRAVE_ACCENT;
	} else if (ls_str_equals(keycode, "Digit1")) {
		return LS_KEY_1;
	} else if (ls_str_equals(keycode, "Digit2")) {
		return LS_KEY_2;
	} else if (ls_str_equals(keycode, "Digit3")) {
		return LS_KEY_3;
	} else if (ls_str_equals(keycode, "Digit4")) {
		return LS_KEY_4;
	} else if (ls_str_equals(keycode, "Digit5")) {
		return LS_KEY_5;
	} else if (ls_str_equals(keycode, "Digit6")) {
		return LS_KEY_6;
	} else if (ls_str_equals(keycode, "Digit7")) {
		return LS_KEY_7;
	} else if (ls_str_equals(keycode, "Digit8")) {
		return LS_KEY_8;
	} else if (ls_str_equals(keycode, "Digit9")) {
		return LS_KEY_9;
	} else if (ls_str_equals(keycode, "Digit0")) {
		return LS_KEY_0;
	} else if (ls_str_equals(keycode, "Minus")) {
		return LS_KEY_MINUS;
	} else if (ls_str_equals(keycode, "Equal")) {
		return LS_KEY_EQUAL;
	} else if (ls_str_equals(keycode, "Backspace")) {
		return LS_KEY_BACKSPACE;
	} else if (ls_str_equals(keycode, "Insert")) {
		return LS_KEY_INSERT;
	} else if (ls_str_equals(keycode, "Home")) {
		return LS_KEY_HOME;
	} else if (ls_str_equals(keycode, "PageUp")) {
		return LS_KEY_PAGE_UP;
	} else if (ls_str_equals(keycode, "NumLock")) {
		return LS_KEY_NUM_LOCK;
	} else if (ls_str_equals(keycode, "NumpadDivide")) {
		return LS_KEY_KP_DIVIDE;
	} else if (ls_str_equals(keycode, "NumpadMultiply")) {
		return LS_KEY_KP_MULTIPLY;
	} else if (ls_str_equals(keycode, "NumpadSubtract")) {
		return LS_KEY_KP_SUBTRACT;
	} else if (ls_str_equals(keycode, "Tab")) {
		return LS_KEY_TAB;
	} else if (ls_str_equals(keycode, "KeyQ")) {
		return LS_KEY_Q;
	} else if (ls_str_equals(keycode, "KeyW")) {
		return LS_KEY_W;
	} else if (ls_str_equals(keycode, "KeyE")) {
		return LS_KEY_E;
	} else if (ls_str_equals(keycode, "KeyR")) {
		return LS_KEY_R;
	} else if (ls_str_equals(keycode, "KeyT")) {
		return LS_KEY_T;
	} else if (ls_str_equals(keycode, "KeyY")) {
		return LS_KEY_Y;
	} else if (ls_str_equals(keycode, "KeyU")) {
		return LS_KEY_U;
	} else if (ls_str_equals(keycode, "KeyI")) {
		return LS_KEY_I;
	} else if (ls_str_equals(keycode, "KeyO")) {
		return LS_KEY_O;
	} else if (ls_str_equals(keycode, "KeyP")) {
		return LS_KEY_P;
	} else if (ls_str_equals(keycode, "BracketLeft")) {
		return LS_KEY_LEFT_BRACKET;
	} else if (ls_str_equals(keycode, "BracketRight")) {
		return LS_KEY_RIGHT_BRACKET;
	} else if (ls_str_equals(keycode, "Backslash")) {
		return LS_KEY_BACKSLASH;
	} else if (ls_str_equals(keycode, "Delete")) {
		return LS_KEY_DELETE;
	} else if (ls_str_equals(keycode, "End")) {
		return LS_KEY_END;
	} else if (ls_str_equals(keycode, "PageDown")) {
		return LS_KEY_PAGE_DOWN;
	} else if (ls_str_equals(keycode, "Numpad7")) {
		return LS_KEY_KP_7;
	} else if (ls_str_equals(keycode, "Numpad8")) {
		return LS_KEY_KP_8;
	} else if (ls_str_equals(keycode, "Numpad9")) {
		return LS_KEY_KP_9;
	} else if (ls_str_equals(keycode, "NumpadAdd")) {
		return LS_KEY_KP_ADD;
	} else if (ls_str_equals(keycode, "CapsLock")) {
		return LS_KEY_CAPS_LOCK;
	} else if (ls_str_equals(keycode, "KeyA")) {
		return LS_KEY_A;
	} else if (ls_str_equals(keycode, "KeyS")) {
		return LS_KEY_S;
	} else if (ls_str_equals(keycode, "KeyD")) {
		return LS_KEY_D;
	} else if (ls_str_equals(keycode, "KeyF")) {
		return LS_KEY_F;
	} else if (ls_str_equals(keycode, "KeyG")) {
		return LS_KEY_G;
	} else if (ls_str_equals(keycode, "KeyH")) {
		return LS_KEY_H;
	} else if (ls_str_equals(keycode, "KeyJ")) {
		return LS_KEY_J;
	} else if (ls_str_equals(keycode, "KeyK")) {
		return LS_KEY_K;
	} else if (ls_str_equals(keycode, "KeyL")) {
		return LS_KEY_L;
	} else if (ls_str_equals(keycode, "Semicolon")) {
		return LS_KEY_SEMICOLON;
	} else if (ls_str_equals(keycode, "Quote")) {
		return LS_KEY_APOSTROPHE;
	} else if (ls_str_equals(keycode, "Enter")) {
		return LS_KEY_ENTER;
	} else if (ls_str_equals(keycode, "Numpad4")) {
		return LS_KEY_KP_4;
	} else if (ls_str_equals(keycode, "Numpad5")) {
		return LS_KEY_KP_5;
	} else if (ls_str_equals(keycode, "Numpad6")) {
		return LS_KEY_KP_6;
	} else if (ls_str_equals(keycode, "ShiftLeft")) {
		return LS_KEY_LEFT_SHIFT;
	} else if (ls_str_equals(keycode, "KeyZ")) {
		return LS_KEY_Z;
	} else if (ls_str_equals(keycode, "KeyX")) {
		return LS_KEY_X;
	} else if (ls_str_equals(keycode, "KeyC")) {
		return LS_KEY_C;
	} else if (ls_str_equals(keycode, "KeyV")) {
		return LS_KEY_V;
	} else if (ls_str_equals(keycode, "KeyB")) {
		return LS_KEY_B;
	} else if (ls_str_equals(keycode, "KeyN")) {
		return LS_KEY_N;
	} else if (ls_str_equals(keycode, "KeyM")) {
		return LS_KEY_M;
	} else if (ls_str_equals(keycode, "Comma")) {
		return LS_KEY_COMMA;
	} else if (ls_str_equals(keycode, "Period")) {
		return LS_KEY_PERIOD;
	} else if (ls_str_equals(keycode, "Slash")) {
		return LS_KEY_SLASH;
	} else if (ls_str_equals(keycode, "ShiftRight")) {
		return LS_KEY_RIGHT_SHIFT;
	} else if (ls_str_equals(keycode, "ArrowUp")) {
		return LS_KEY_UP;
	} else if (ls_str_equals(keycode, "Numpad1")) {
		return LS_KEY_KP_1;
	} else if (ls_str_equals(keycode, "Numpad2")) {
		return LS_KEY_KP_2;
	} else if (ls_str_equals(keycode, "Numpad3")) {
		return LS_KEY_KP_3;
	} else if (ls_str_equals(keycode, "ControlLeft")) {
		return LS_KEY_LEFT_CONTROL;
	} else if (ls_str_equals(keycode, "MetaLeft")) {
		return LS_KEY_LEFT_SUPER;
	} else if (ls_str_equals(keycode, "AltLeft")) {
		return LS_KEY_LEFT_ALT;
	} else if (ls_str_equals(keycode, "Space")) {
		return LS_KEY_SPACE;
	} else if (ls_str_equals(keycode, "AltRight")) {
		return LS_KEY_RIGHT_ALT;
	} else if (ls_str_equals(keycode, "MetaRight")) {
		return LS_KEY_RIGHT_SUPER;
	} else if (ls_str_equals(keycode, "ContextMenu")) {
		return LS_KEY_MENU;
	} else if (ls_str_equals(keycode, "ControlRight")) {
		return LS_KEY_RIGHT_CONTROL;
	} else if (ls_str_equals(keycode, "ArrowLeft")) {
		return LS_KEY_LEFT;
	} else if (ls_str_equals(keycode, "ArrowDown")) {
		return LS_KEY_DOWN;
	} else if (ls_str_equals(keycode, "ArrowRight")) {
		return LS_KEY_RIGHT;
	} else if (ls_str_equals(keycode, "Numpad0")) {
		return LS_KEY_KP_0;
	} else if (ls_str_equals(keycode, "NumpadDecimal")) {
		return LS_KEY_KP_DECIMAL;
	} else if (ls_str_equals(keycode, "NumpadEnter")) {
		return LS_KEY_KP_ENTER;
	}

	return LS_KEY_UNKNOWN;
}

_FORCE_INLINE_ LSMouseButton web_map_mbutton(uint32 keycode) {
	switch (keycode) {
		case 0:
			return LS_MOUSE_BUTTON_LEFT;
		case 1:
			return LS_MOUSE_BUTTON_RIGHT;
		case 2:
			return LS_MOUSE_BUTTON_MIDDLE;
		default:
			break;
	}
	return LS_MOUSE_BUTTON_NONE;
}

#endif // WEB_KEYMAP_H