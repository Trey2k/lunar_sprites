#ifndef KEYCODES_H
#define KEYCODES_H

#include "core/string.h"
#include "core/typedefs.h"

/* Keycodes are from glfw */
typedef enum {
	LS_KEY_UNKNOWN = -1,
	LS_KEY_NONE = 0,

	/* Printable keys */
	LS_KEY_SPACE = 32,
	LS_KEY_APOSTROPHE = 39, /* ' */
	LS_KEY_COMMA = 44, /* , */
	LS_KEY_MINUS = 45, /* - */
	LS_KEY_PERIOD = 46, /* . */
	LS_KEY_SLASH = 47, /* / */
	LS_KEY_0 = 48,
	LS_KEY_1 = 49,
	LS_KEY_2 = 50,
	LS_KEY_3 = 51,
	LS_KEY_4 = 52,
	LS_KEY_5 = 53,
	LS_KEY_6 = 54,
	LS_KEY_7 = 55,
	LS_KEY_8 = 56,
	LS_KEY_9 = 57,
	LS_KEY_SEMICOLON = 59, /* ; */
	LS_KEY_EQUAL = 61, /* = */
	LS_KEY_A = 65,
	LS_KEY_B = 66,
	LS_KEY_C = 67,
	LS_KEY_D = 68,
	LS_KEY_E = 69,
	LS_KEY_F = 70,
	LS_KEY_G = 71,
	LS_KEY_H = 72,
	LS_KEY_I = 73,
	LS_KEY_J = 74,
	LS_KEY_K = 75,
	LS_KEY_L = 76,
	LS_KEY_M = 77,
	LS_KEY_N = 78,
	LS_KEY_O = 79,
	LS_KEY_P = 80,
	LS_KEY_Q = 81,
	LS_KEY_R = 82,
	LS_KEY_S = 83,
	LS_KEY_T = 84,
	LS_KEY_U = 85,
	LS_KEY_V = 86,
	LS_KEY_W = 87,
	LS_KEY_X = 88,
	LS_KEY_Y = 89,
	LS_KEY_Z = 90,
	LS_KEY_LEFT_BRACKET = 91, /* [ */
	LS_KEY_BACKSLASH = 92, /* \ */
	LS_KEY_RIGHT_BRACKET = 93, /* ] */
	LS_KEY_GRAVE_ACCENT = 96, /* ` */
	LS_KEY_WORLD_1 = 161, /* non-US #1 */
	LS_KEY_WORLD_2 = 162, /* non-US #2 */

	/* Function keys */
	LS_KEY_ESCAPE = 256,
	LS_KEY_ENTER = 257,
	LS_KEY_TAB = 258,
	LS_KEY_BACKSPACE = 259,
	LS_KEY_INSERT = 260,
	LS_KEY_DELETE = 261,
	LS_KEY_RIGHT = 262,
	LS_KEY_LEFT = 263,
	LS_KEY_DOWN = 264,
	LS_KEY_UP = 265,
	LS_KEY_PAGE_UP = 266,
	LS_KEY_PAGE_DOWN = 267,
	LS_KEY_HOME = 268,
	LS_KEY_END = 269,
	LS_KEY_CAPS_LOCK = 280,
	LS_KEY_SCROLL_LOCK = 281,
	LS_KEY_NUM_LOCK = 282,
	LS_KEY_PRINT_SCREEN = 283,
	LS_KEY_PAUSE = 284,
	LS_KEY_F1 = 290,
	LS_KEY_F2 = 291,
	LS_KEY_F3 = 292,
	LS_KEY_F4 = 293,
	LS_KEY_F5 = 294,
	LS_KEY_F6 = 295,
	LS_KEY_F7 = 296,
	LS_KEY_F8 = 297,
	LS_KEY_F9 = 298,
	LS_KEY_F10 = 299,
	LS_KEY_F11 = 300,
	LS_KEY_F12 = 301,
	LS_KEY_F13 = 302,
	LS_KEY_F14 = 303,
	LS_KEY_F15 = 304,
	LS_KEY_F16 = 305,
	LS_KEY_F17 = 306,
	LS_KEY_F18 = 307,
	LS_KEY_F19 = 308,
	LS_KEY_F20 = 309,
	LS_KEY_F21 = 310,
	LS_KEY_F22 = 311,
	LS_KEY_F23 = 312,
	LS_KEY_F24 = 313,
	LS_KEY_F25 = 314,
	LS_KEY_KP_0 = 320,
	LS_KEY_KP_1 = 321,
	LS_KEY_KP_2 = 322,
	LS_KEY_KP_3 = 323,
	LS_KEY_KP_4 = 324,
	LS_KEY_KP_5 = 325,
	LS_KEY_KP_6 = 326,
	LS_KEY_KP_7 = 327,
	LS_KEY_KP_8 = 328,
	LS_KEY_KP_9 = 329,
	LS_KEY_KP_DECIMAL = 330,
	LS_KEY_KP_DIVIDE = 331,
	LS_KEY_KP_MULTIPLY = 332,
	LS_KEY_KP_SUBTRACT = 333,
	LS_KEY_KP_ADD = 334,
	LS_KEY_KP_ENTER = 335,
	LS_KEY_KP_EQUAL = 336,
	LS_KEY_LEFT_SHIFT = 340,
	LS_KEY_LEFT_CONTROL = 341,
	LS_KEY_LEFT_ALT = 342,
	LS_KEY_LEFT_SUPER = 343,
	LS_KEY_RIGHT_SHIFT = 344,
	LS_KEY_RIGHT_CONTROL = 345,
	LS_KEY_RIGHT_ALT = 346,
	LS_KEY_RIGHT_SUPER = 347,
	LS_KEY_MENU = 348,

	LS_KEY_LAST = LS_KEY_MENU
} LSKeycode;

typedef enum {
	LS_MOD_NONE = 0,
	LS_MOD_SHIFT = 0x0001,
	LS_MOD_CONTROL = 0x0002,
	LS_MOD_ALT = 0x0004,
	LS_MOD_SUPER = 0x0008,
	LS_MOD_CAPS_LOCK = 0x0010,
	LS_MOD_NUM_LOCK = 0x0020,
	LS_MOD_LAST = LS_MOD_NUM_LOCK
} LSModifier;

typedef enum {
	LS_MOUSE_BUTTON_NONE = -1,
	LS_MOUSE_BUTTON_1 = 0,
	LS_MOUSE_BUTTON_2 = 1,
	LS_MOUSE_BUTTON_3 = 2,
	LS_MOUSE_BUTTON_4 = 3,
	LS_MOUSE_BUTTON_5 = 4,
	LS_MOUSE_BUTTON_6 = 5,
	LS_MOUSE_BUTTON_7 = 6,
	LS_MOUSE_BUTTON_8 = 7,
	LS_MOUSE_BUTTON_LAST = LS_MOUSE_BUTTON_8,
	LS_MOUSE_BUTTON_LEFT = LS_MOUSE_BUTTON_1,
	LS_MOUSE_BUTTON_RIGHT = LS_MOUSE_BUTTON_2,
	LS_MOUSE_BUTTON_MIDDLE = LS_MOUSE_BUTTON_3,
} LSMouseButton;

_FORCE_INLINE_ const String keycode_to_string(LSKeycode keycode) {
	switch (keycode) {
		case LS_KEY_SPACE:
			return "[Space]";
		case LS_KEY_APOSTROPHE:
			return "'";
		case LS_KEY_COMMA:
			return ",";
		case LS_KEY_MINUS:
			return "-";
		case LS_KEY_PERIOD:
			return ".";
		case LS_KEY_SLASH:
			return "/";
		case LS_KEY_0:
			return "0";
		case LS_KEY_1:
			return "1";
		case LS_KEY_2:
			return "2";
		case LS_KEY_3:
			return "3";
		case LS_KEY_4:
			return "4";
		case LS_KEY_5:
			return "5";
		case LS_KEY_6:
			return "6";
		case LS_KEY_7:
			return "7";
		case LS_KEY_8:
			return "8";
		case LS_KEY_9:
			return "9";
		case LS_KEY_SEMICOLON:
			return ";";
		case LS_KEY_EQUAL:
			return "=";
		case LS_KEY_A:
			return "A";
		case LS_KEY_B:
			return "B";
		case LS_KEY_C:
			return "C";
		case LS_KEY_D:
			return "D";
		case LS_KEY_E:
			return "E";
		case LS_KEY_F:
			return "F";
		case LS_KEY_G:
			return "G";
		case LS_KEY_H:
			return "H";
		case LS_KEY_I:
			return "I";
		case LS_KEY_J:
			return "J";
		case LS_KEY_K:
			return "K";
		case LS_KEY_L:
			return "L";
		case LS_KEY_M:
			return "M";
		case LS_KEY_N:
			return "N";
		case LS_KEY_O:
			return "O";
		case LS_KEY_P:
			return "P";
		case LS_KEY_Q:
			return "Q";
		case LS_KEY_R:
			return "R";
		case LS_KEY_S:
			return "S";
		case LS_KEY_T:
			return "T";
		case LS_KEY_U:
			return "U";
		case LS_KEY_V:
			return "V";
		case LS_KEY_W:
			return "W";
		case LS_KEY_X:
			return "X";
		case LS_KEY_Y:
			return "Y";
		case LS_KEY_Z:
			return "Z";
		case LS_KEY_LEFT_BRACKET:
			return "[";
		case LS_KEY_BACKSLASH:
			return "\\";
		case LS_KEY_RIGHT_BRACKET:
			return "]";
		case LS_KEY_GRAVE_ACCENT:
			return "`";
		case LS_KEY_WORLD_1:
			return "[World 1]";
		case LS_KEY_WORLD_2:
			return "[World 2]";
		case LS_KEY_ESCAPE:
			return "[Escape]";
		case LS_KEY_ENTER:
			return "[Enter]";
		case LS_KEY_TAB:
			return "[Tab]";
		case LS_KEY_BACKSPACE:
			return "[Backspace]";
		case LS_KEY_INSERT:
			return "[Insert]";
		case LS_KEY_DELETE:
			return "[Delete]";
		case LS_KEY_RIGHT:
			return "[Right]";
		case LS_KEY_LEFT:
			return "[Left]";
		case LS_KEY_DOWN:
			return "[Down]";
		case LS_KEY_UP:
			return "[Up]";
		case LS_KEY_PAGE_UP:
			return "[Page Up]";
		case LS_KEY_PAGE_DOWN:
			return "[Page Down]";
		case LS_KEY_HOME:
			return "[Home]";
		case LS_KEY_END:
			return "[End]";
		case LS_KEY_CAPS_LOCK:
			return "[Caps Lock]";
		case LS_KEY_SCROLL_LOCK:
			return "[Scroll Lock]";
		case LS_KEY_NUM_LOCK:
			return "[Num Lock]";
		case LS_KEY_PRINT_SCREEN:
			return "[Print Screen]";
		case LS_KEY_PAUSE:
			return "[Pause]";
		case LS_KEY_F1:
			return "[F1]";
		case LS_KEY_F2:
			return "[F2]";
		case LS_KEY_F3:
			return "[F3]";
		case LS_KEY_F4:
			return "[F4]";
		case LS_KEY_F5:
			return "[F5]";
		case LS_KEY_F6:
			return "[F6]";
		case LS_KEY_F7:
			return "[F7]";
		case LS_KEY_F8:
			return "[F8]";
		case LS_KEY_F9:
			return "[F9]";
		case LS_KEY_F10:
			return "[F10]";
		case LS_KEY_F11:
			return "[F11]";
		case LS_KEY_F12:
			return "[F12]";
		case LS_KEY_F13:
			return "[F13]";
		case LS_KEY_F14:
			return "[F14]";
		case LS_KEY_F15:
			return "[F15]";
		case LS_KEY_F16:
			return "[F16]";
		case LS_KEY_F17:
			return "[F17]";
		case LS_KEY_F18:
			return "[F18]";
		case LS_KEY_F19:
			return "[F19]";
		case LS_KEY_F20:
			return "[F20]";
		case LS_KEY_F21:
			return "[F21]";
		case LS_KEY_F22:
			return "[F22]";
		case LS_KEY_F23:
			return "[F23]";
		case LS_KEY_F24:
			return "[F24]";
		case LS_KEY_F25:
			return "[F25]";
		case LS_KEY_KP_0:
			return "[KEYPAD] 0";
		case LS_KEY_KP_1:
			return "[KEYPAD] 1";
		case LS_KEY_KP_2:
			return "[KEYPAD] 2";
		case LS_KEY_KP_3:
			return "[KEYPAD] 3";
		case LS_KEY_KP_4:
			return "[KEYPAD] 4";
		case LS_KEY_KP_5:
			return "[KEYPAD] 5";
		case LS_KEY_KP_6:
			return "[KEYPAD] 6";
		case LS_KEY_KP_7:
			return "[KEYPAD] 7";
		case LS_KEY_KP_8:
			return "[KEYPAD] 8";
		case LS_KEY_KP_9:
			return "[KEYPAD] 9";
		case LS_KEY_KP_DECIMAL:
			return "[KEYPAD] .";
		case LS_KEY_KP_DIVIDE:
			return "[KEYPAD] /";
		case LS_KEY_KP_MULTIPLY:
			return "[KEYPAD] *";
		case LS_KEY_KP_SUBTRACT:
			return "[KEYPAD] -";
		case LS_KEY_KP_ADD:
			return "[KEYPAD] +";
		case LS_KEY_KP_ENTER:
			return "[KEYPAD] [Enter]";
		case LS_KEY_KP_EQUAL:
			return "[KEYPAD] =";
		case LS_KEY_LEFT_SHIFT:
			return "[Left Shift]";
		case LS_KEY_LEFT_CONTROL:
			return "[Left Control]";
		case LS_KEY_LEFT_ALT:
			return "[Left Alt]";
		case LS_KEY_LEFT_SUPER:
			return "[Left Super]";
		case LS_KEY_RIGHT_SHIFT:
			return "[Right Shift]";
		case LS_KEY_RIGHT_CONTROL:
			return "[Right Control]";
		case LS_KEY_RIGHT_ALT:
			return "[Right Alt]";
		case LS_KEY_RIGHT_SUPER:
			return "[Right Super]";
		case LS_KEY_MENU:
			return "[Menu]";
		default:
			return "[Unknown]";
	};
}

_FORCE_INLINE_ const String modifier_to_string(LSModifier modifier) {
	switch (modifier) {
		case LS_MOD_SHIFT:
			return "[Shift]";
		case LS_MOD_CONTROL:
			return "[Control]";
		case LS_MOD_ALT:
			return "[Alt]";
		case LS_MOD_SUPER:
			return "[Super]";
		case LS_MOD_CAPS_LOCK:
			return "[Caps Lock]";
		case LS_MOD_NUM_LOCK:
			return "[Num Lock]";
		default:
			return "[Unknown]";
	};
}

_FORCE_INLINE_ const String mouse_button_to_string(LSMouseButton button) {
	switch (button) {
		case LS_MOUSE_BUTTON_1:
			return "[Mouse 1]";
		case LS_MOUSE_BUTTON_2:
			return "[Mouse 2]";
		case LS_MOUSE_BUTTON_3:
			return "[Mouse 3]";
		case LS_MOUSE_BUTTON_4:
			return "[Mouse 4]";
		case LS_MOUSE_BUTTON_5:
			return "[Mouse 5]";
		case LS_MOUSE_BUTTON_6:
			return "[Mouse 6]";
		case LS_MOUSE_BUTTON_7:
			return "[Mouse 7]";
		case LS_MOUSE_BUTTON_8:
			return "[Mouse 8]";
		default:
			return "[Unknown]";
	};
}

#endif // KEYCODES_H