#include "renderer/renderer.h"

#include "renderer/renderer_interface.h"

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

struct Renderer {
	LSCore *core;

	RendererInterface interface;

	FlagValue *backend_flag;
	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLRenderer *opengl;
#endif
	};
};

static void check_flags(Renderer *renderer);

Renderer *renderer_create(LSCore *core) {
	Renderer *renderer = ls_malloc(sizeof(Renderer));
	renderer->core = core;

	renderer->backend_flag = flag_manager_register(core_get_flag_manager(core),
			"renderer-backend", FLAG_TYPE_STRING, FLAG_VAL(str, "OPENGL"),
			"The renderer backend to use. Valid values are NONE and OPENGL.");

	return renderer;
}

void renderer_start(Renderer *renderer) {
	check_flags(renderer);
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			renderer->opengl = opengl_renderer_create(renderer->core);
			opengl_register_methods(&renderer->interface);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", renderer->backend);
	};
}

void renderer_destroy(Renderer *renderer) {
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

RendererBackend renderer_get_backend(const Renderer *renderer) {
	return renderer->backend;
}

LSCore *renderer_get_core(const Renderer *renderer) {
	return renderer->core;
}

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl(const Renderer *renderer) {
	return renderer->opengl;
}
#endif

void renderer_set_clear_color(const Renderer *renderer, float32 r, float32 g, float32 b, float32 a) {
	renderer->interface.set_clear_color(r, g, b, a);
}

void renderer_clear(const Renderer *renderer) {
	renderer->interface.clear();
}

static void check_flags(Renderer *renderer) {
	ls_str_to_upper(renderer->backend_flag->str);

	if (ls_str_equals(renderer->backend_flag->str, "NONE")) {
		renderer->backend = RENDERER_BACKEND_NONE;
	} else if (ls_str_equals(renderer->backend_flag->str, "OPENGL")) {
		renderer->backend = RENDERER_BACKEND_OPENGL;
	} else {
		ls_log_fatal("Invalid renderer backend: %s\n", renderer->backend_flag->str);
	}
}