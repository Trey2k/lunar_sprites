#include "renderer/renderer.h"
#include "core/debug.h"
#include "core/flags.h"
#include "core/memory.h"
#include "core/types/typedefs.h"

struct Renderer {
	const OS *os;

	FlagValue *backend_flag;

	RendererBackend backend;
};

static struct Renderer renderer;

static void check_flags();

void renderer_init() {
	renderer.backend_flag = ls_register_flag("renderer-backend", FLAG_TYPE_STRING, (FlagValue){ .string = "OPENGL" },
			"The renderer backend to use. Valid values are NONE and OPENGL.");
#if defined(OPENGL_ENABLED)
	opengl_renderer_init();
#endif
}

void renderer_start(const OS *os) {
	renderer.os = os;
	check_flags();
	switch (renderer.backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_renderer_start(os);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", renderer.backend);
	};
}

void renderer_deinit() {
	switch (renderer.backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			opengl_renderer_deinit();
#endif

		} break;
	}
}

RendererBackend renderer_get_backend() {
	return renderer.backend;
}

static void check_flags() {
	ls_str_to_upper(renderer.backend_flag->string);

	if (ls_str_equals(renderer.backend_flag->string, "NONE")) {
		renderer.backend = RENDERER_BACKEND_NONE;
	} else if (ls_str_equals(renderer.backend_flag->string, "OPENGL")) {
		renderer.backend = RENDERER_BACKEND_OPENGL;
	} else {
		ls_log_fatal("Invalid renderer backend: %s\n", renderer.backend_flag->string);
	}
}