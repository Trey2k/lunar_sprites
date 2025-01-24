#include "renderer/context.h"
#include "core/debug.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "renderer/renderer.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/context.h"
#endif

struct Context {
	const LSWindow *window;

	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLContext *opengl_context;
#endif
	};
};

Context *renderer_context_create(LSWindow *window) {
	Context *context = ls_malloc(sizeof(Context));
	context->backend = renderer_get_backend();
	context->window = window;

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			context->opengl_context = opengl_context_create(renderer_get_opengl(), window);
			LS_ASSERT(context->opengl_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};

	return context;
}

void renderer_context_destroy(Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_context_destroy(context->opengl_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};

	ls_free(context);
}

void renderer_context_make_current(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_context_make_current(context->opengl_context);
			renderer_set_active_context(context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

void renderer_context_detach(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_context_detach(context->opengl_context);
			renderer_set_active_context( NULL);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

void renderer_context_swap_buffers(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_context_swap_buffers(context->opengl_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

void renderer_context_resize(const Context *context, Vector2u size) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			opengl_context_resize(context->opengl_context, size);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

Vector2u renderer_context_get_size(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			return opengl_context_get_size(context->opengl_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
			return vec2u(0, 0);
	};
}