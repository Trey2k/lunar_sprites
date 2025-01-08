#ifndef WEB_KEYMAP_H
#define WEB_KEYMAP_H

#include <emscripten/html5.h>

#include "core/input/keycodes.h"
#include "core/types/typedefs.h"

void keymap_init();
void keymap_deinit();

LSKeycode web_map_key(String keycode);

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