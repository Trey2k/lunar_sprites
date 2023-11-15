#include "platform/web/window.h"
#include "platform/os.h"
#include "platform/web/js_methods.h"
#include "platform/web/keymap.h"
#include "platform/window.h"

#include "renderer/renderer.h"
#include "renderer/window.h"

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "platform/web/webgl.h"
#endif // OPENGL_ENABLED

static bool root_window_created = false;
static int32 sub_window_count = 0;

static int keydown_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);
static int keyup_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);

static int mousedown_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
static int mouseup_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
static int mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
static int mouseenter_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
static int mouseleave_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);

static void register_callbacks(PlatformWindow *window);

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));

	window->title = config.title;
	window->width = config.size.x;
	window->height = config.size.y;
	window->position = config.position;
	window->root_window = config.root_window;
	window->hidden = false;
	window->fullscreen = false;
	window->os = os;

	window->keydown_codes = slice_create(16, false);
	window->keyup_codes = slice_create(16, false);

	window->mousedown_codes = slice32_create(16);
	window->mousedown_positions = slice128_create(16);
	window->mouseup_codes = slice32_create(16);
	window->mouseup_positions = slice128_create(16);

	window->mouse_move_positions = slice128_create(16);

	window->mouse_leave_positions = slice128_create(16);
	window->mouse_enter_positions = slice128_create(16);

	if (window->root_window) {
		if (root_window_created) {
			ls_log(LOG_LEVEL_ERROR, "Root window already created!\n");
			ls_free(window);
			return NULL;
		}

		root_window_created = true;

		window->canvas_id = ls_str_copy("#root_window");
	} else {
		window->canvas_id = ls_str_format("#window_%d", sub_window_count++);
	}

	RendererBackend renderer_backend = renderer_get_backend();
	switch (renderer_backend) {
		case RENDERER_BACKEND_NONE:
			break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			webgl_init_canvas(window);
		} break;

#endif // OPENGL_ENABLED

		default:
			ls_log(LOG_LEVEL_WARNING, "Unknown renderer backend: %d\n", renderer_backend);
			break;
	};

	platform_window_set_fullscreen(window, config.fullscreen);

	if (config.hidden) {
		platform_window_hide(window);
	}

	register_callbacks(window);

	// TODO: Should be optional
	set_canvas_focused(window->canvas_id, true);

	return window;
}

void platform_destroy_window(PlatformWindow *window) {
	RendererBackend renderer_backend = renderer_get_backend();
	switch (renderer_backend) {
		case RENDERER_BACKEND_NONE:
			break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			webgl_deinit_canvas(window);
		} break;

#endif // OPENGL_ENABLED

		default:
			ls_log(LOG_LEVEL_WARNING, "Unknown renderer backend: %d\n", renderer_backend);
			break;
	};

	ls_free(window->canvas_id);
	ls_free(window);
}

void platform_window_poll(PlatformWindow *window) {
	LS_ASSERT(window);

	for (int32 i = 0; i < slice_get_size(window->keydown_codes); i++) {
		String code = slice_get(window->keydown_codes, i).str;
		core_handle_press(web_map_key(code));
	}
	slice_clear(window->keydown_codes);

	for (int32 i = 0; i < slice_get_size(window->keyup_codes); i++) {
		String code = slice_get(window->keyup_codes, i).str;
		core_handle_release(web_map_key(code));
	}
	slice_clear(window->keyup_codes);

	for (int32 i = 0; i < slice32_get_size(window->mousedown_codes); i++) {
		uint32 code = slice32_get(window->mousedown_codes, i).u32;
		Vector2i position = slice128_get(window->mousedown_positions, i).v2i;
		core_handle_mouse_press(web_map_mbutton(code), position);
	}
	slice32_clear(window->mousedown_codes);
	slice128_clear(window->mousedown_positions);

	for (int32 i = 0; i < slice32_get_size(window->mouseup_codes); i++) {
		uint32 code = slice32_get(window->mouseup_codes, i).u32;
		Vector2i position = slice128_get(window->mouseup_positions, i).v2i;
		core_handle_mouse_release(web_map_mbutton(code), position);
	}
	slice32_clear(window->mouseup_codes);
	slice128_clear(window->mouseup_positions);

	for (int32 i = 0; i < slice128_get_size(window->mouse_move_positions); i++) {
		Vector2i position = slice128_get(window->mouse_move_positions, i).v2i;
		core_handle_mouse_move(position);
	}
	slice128_clear(window->mouse_move_positions);

	for (int32 i = 0; i < slice128_get_size(window->mouse_leave_positions); i++) {
		Vector2i position = slice128_get(window->mouse_leave_positions, i).v2i;
		core_handle_mouse_leave(position);
	}
	slice128_clear(window->mouse_leave_positions);

	for (int32 i = 0; i < slice128_get_size(window->mouse_enter_positions); i++) {
		Vector2i position = slice128_get(window->mouse_enter_positions, i).v2i;
		core_handle_mouse_enter(position);
	}
	slice128_clear(window->mouse_enter_positions);
}

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window) {
	return 0;
}

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen) {
	LS_ASSERT(window);

	if (window->fullscreen == fullscreen) {
		return;
	}

	window->fullscreen = fullscreen;

	set_canvas_fullscreen(window->canvas_id, fullscreen);
}

void platform_window_show(PlatformWindow *window) {
	LS_ASSERT(window);

	if (!window->hidden) {
		return;
	}

	window->hidden = false;

	set_canvas_visibility(window->canvas_id, !window->hidden);
}

void platform_window_hide(PlatformWindow *window) {
	LS_ASSERT(window);

	if (window->hidden) {
		return;
	}

	window->hidden = true;

	set_canvas_visibility(window->canvas_id, !window->hidden);
}

void platform_window_set_title(PlatformWindow *window, const char *title) {
	LS_ASSERT(window);

	window->title = title;
}

void platform_window_set_size(PlatformWindow *window, int32 width, int32 height) {
	LS_ASSERT(window);

	window->width = width;
	window->height = height;

	set_canvas_size(window->canvas_id, width, height);
}

bool platform_window_is_visible(const PlatformWindow *window) {
	LS_ASSERT(window);

	return !window->hidden;
}

bool platform_window_is_fullscreen(const PlatformWindow *window) {
	LS_ASSERT(window);

	return window->fullscreen;
}

static void register_callbacks(PlatformWindow *window) {
	LS_ASSERT(window);

	emscripten_set_keydown_callback(window->canvas_id, window, true, keydown_callback);
	emscripten_set_keyup_callback(window->canvas_id, window, true, keyup_callback);

	emscripten_set_mousedown_callback(window->canvas_id, window, true, mousedown_callback);
	emscripten_set_mouseup_callback(window->canvas_id, window, true, mouseup_callback);
	emscripten_set_mousemove_callback(window->canvas_id, window, true, mousemove_callback);
	emscripten_set_mouseenter_callback(window->canvas_id, window, true, mouseenter_callback);
	emscripten_set_mouseleave_callback(window->canvas_id, window, true, mouseleave_callback);

	ls_printf("Registered callbacks for window: %s\n", window->canvas_id);
}

static int keydown_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	PlatformWindow *window = userData;

	slice_append(window->keydown_codes, SLICE_VAL(str, e->code));
	return 0;
}

static int keyup_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	PlatformWindow *window = userData;

	slice_append(window->keyup_codes, SLICE_VAL(str, e->code));
	return 0;
}

static int mousedown_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	slice32_append(window->mousedown_codes, SLICE_VAL32(u32, e->button));
	Vector2i pos = (Vector2i){ e->clientX, e->clientY };
	slice128_append(window->mousedown_positions, SLICE_VAL128(v2i, pos));
	return 0;
}

static int mouseup_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	slice32_append(window->mouseup_codes, SLICE_VAL32(u32, e->button));
	Vector2i pos = (Vector2i){ e->clientX, e->clientY };
	slice128_append(window->mouseup_positions, SLICE_VAL128(v2i, pos));
	return 0;
}

static int mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	Vector2i pos = (Vector2i){ e->clientX, e->clientY };
	slice128_append(window->mouse_move_positions, SLICE_VAL128(v2i, pos));
	return 0;
}

static int mouseenter_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	Vector2i pos = (Vector2i){ e->clientX, e->clientY };
	slice128_append(window->mouse_enter_positions, SLICE_VAL128(v2i, pos));
	return 0;
}

static int mouseleave_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	Vector2i pos = (Vector2i){ e->clientX, e->clientY };
	slice128_append(window->mouse_leave_positions, SLICE_VAL128(v2i, pos));
	return 0;
}