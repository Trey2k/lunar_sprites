#include "renderer/renderer.h"

#include "renderer.h"
#include "renderer/context.h"
#include "renderer/renderer_interface.h"
#include "renderer/sprite_object.h"
#include "renderer/texture.h"
#include "renderer/window.h"

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

typedef struct {
	RendererInterface interface;

	const LSWindow *active_window;

	const Context *active_context;

	FlagValue *backend_flag;
	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLRenderer *opengl;
#endif
	};
} Renderer;

static Renderer *renderer = NULL;

static void check_flags();

void renderer_init() {
	renderer = ls_malloc(sizeof(Renderer));

	renderer->active_window = NULL;
	renderer->active_context = NULL;

	renderer->backend_flag = flag_manager_register("renderer-backend", FLAG_TYPE_STRING, FLAG_VAL(str, "OPENGL"),
			"The renderer backend to use. Valid values are NONE and OPENGL.");

	texture_manager_init();
	register_sprite_object();
}

void renderer_start() {
	check_flags();
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			renderer->opengl = opengl_renderer_create();
			opengl_register_methods(&renderer->interface);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", renderer->backend);
	};
}

void renderer_deinit() {
	texture_manager_deinit();

	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			opengl_renderer_destroy(renderer->opengl);
#endif

		} break;
	}
}

RendererBackend renderer_get_backend() {
	return renderer->backend;
}

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl() {
	return renderer->opengl;
}
#endif

void renderer_set_clear_color(float32 r, float32 g, float32 b, float32 a) {
	renderer->interface.set_clear_color(r, g, b, a);
}

void renderer_clear() {
	renderer->interface.clear();

	Vector2u viewport_size = window_get_size(renderer->active_window);
	if (!vec2u_equals(viewport_size, renderer_context_get_size(renderer->active_context))) {
		renderer_context_resize(renderer->active_context, viewport_size);
	}
}

void renderer_set_active_window(const LSWindow *window) {
	renderer->active_window = window;
}

void renderer_set_active_context(const Context *context) {
	renderer->active_context = context;
}

Vector2u renderer_get_viewport_size() {
	if (!renderer->active_context) {
		return vec2u(0, 0);
	}

	return renderer_context_get_size(renderer->active_context);
}

static void check_flags() {
	ls_str_to_upper(renderer->backend_flag->str);

	if (ls_str_equals(renderer->backend_flag->str, "NONE")) {
		renderer->backend = RENDERER_BACKEND_NONE;
	} else if (ls_str_equals(renderer->backend_flag->str, "OPENGL")) {
		renderer->backend = RENDERER_BACKEND_OPENGL;
	} else {
		ls_log_fatal("Invalid renderer backend: %s\n", renderer->backend_flag->str);
	}
}