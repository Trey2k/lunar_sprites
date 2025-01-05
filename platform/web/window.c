#include "platform/web/window.h"
#include "platform/web/js_methods.h"
#include "platform/web/keymap.h"
#include "platform/web/os.h"
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

typedef enum {
	WEBW_EVENT_NONE,
	WEBW_EVENT_KEYDOWN,
	WEBW_EVENT_KEYUP,
	WEBW_EVENT_MOUSEDOWN,
	WEBW_EVENT_MOUSEUP,
	WEBW_EVENT_MOUSEMOVE,
	WEBW_EVENT_MOUSEENTER,
	WEBW_EVENT_MOUSELEAVE,
} WebWindowEventType;

typedef struct {
	WebWindowEventType type;

	union {
		LSKeycode keycode;
		LSMouseButton mbutton;
	};

	Vector2i position;

} WebWindowEvent;

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config, const Renderer *renderer, const LSWindow *parent) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));

	window->title = config.title;
	window->width = config.size.x;
	window->height = config.size.y;
	window->position = config.position;
	window->root_window = config.root_window;
	window->hidden = false;
	window->fullscreen = false;
	window->os = os;
	window->renderer = renderer;
	window->input_manager = os->input_manager;

	window->events = slice_create(16, true);

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

	RendererBackend renderer_backend = renderer_get_backend(renderer);
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
	RendererBackend renderer_backend = renderer_get_backend(window->renderer);
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

	for (int32 i = 0; i < slice_get_size(window->events); i++) {
		WebWindowEvent *event = slice_get(window->events, i).ptr;
		LS_ASSERT(event);

		switch (event->type) {
			case WEBW_EVENT_KEYDOWN: {
				input_handle_press(window->input_manager, event->keycode);
			} break;

			case WEBW_EVENT_KEYUP: {
				input_handle_release(window->input_manager, event->keycode);
			} break;

			case WEBW_EVENT_MOUSEDOWN: {
				input_handle_mouse_press(window->input_manager, event->mbutton, event->position);
			} break;

			case WEBW_EVENT_MOUSEUP: {
				input_handle_mouse_release(window->input_manager, event->mbutton, event->position);
			} break;

			case WEBW_EVENT_MOUSEMOVE: {
				input_handle_mouse_move(window->input_manager, event->position);
			} break;

			case WEBW_EVENT_MOUSEENTER: {
				input_handle_mouse_enter(window->input_manager, event->position);
			} break;

			case WEBW_EVENT_MOUSELEAVE: {
				input_handle_mouse_leave(window->input_manager, event->position);
			} break;

			default:
				ls_log(LOG_LEVEL_WARNING, "Unknown event type: %d\n", event->type);
				break;
		}
	}
	slice_clear(window->events);
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

Vector2u platform_window_get_size(const PlatformWindow *window) {
	LS_ASSERT(window);

	return vec2u(window->width, window->height);
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
}

static int keydown_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_KEYDOWN;
	web_event->keycode = web_map_key(e->code);

	slice_append(window->events, SLICE_VAL(ptr, web_event));

	return 0;
}

static int keyup_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_KEYUP;
	web_event->keycode = web_map_key(e->code);

	slice_append(window->events, SLICE_VAL(ptr, web_event));

	return 0;
}

static int mousedown_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_MOUSEDOWN;
	web_event->mbutton = web_map_mbutton(e->button);
	web_event->position = vec2u(e->clientX, e->clientY);

	slice_append(window->events, SLICE_VAL(ptr, web_event));

	return 0;
}

static int mouseup_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_MOUSEUP;
	web_event->mbutton = web_map_mbutton(e->button);
	web_event->position = vec2u(e->clientX, e->clientY);

	slice_append(window->events, SLICE_VAL(ptr, web_event));
	return 0;
}

static int mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_MOUSEMOVE;
	web_event->position = vec2u(e->clientX, e->clientY);

	slice_append(window->events, SLICE_VAL(ptr, web_event));
	return 0;
}

static int mouseenter_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_MOUSEENTER;
	web_event->position = vec2u(e->clientX, e->clientY);

	slice_append(window->events, SLICE_VAL(ptr, web_event));
	return 0;
}

static int mouseleave_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
	PlatformWindow *window = userData;

	WebWindowEvent *web_event = ls_malloc(sizeof(WebWindowEvent));

	web_event->type = WEBW_EVENT_MOUSELEAVE;
	web_event->position = vec2u(e->clientX, e->clientY);

	slice_append(window->events, SLICE_VAL(ptr, web_event));
	return 0;
}