#include "renderer/context.h"
#include "core/debug.h"
#include "core/memory/memory.h"
#include "core/types/typedefs.h"
#include "renderer/renderer.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/context.h"
#endif

Context *renderer_context_create(const Renderer *renderer, const LSWindow *window) {
	const RendererBackend backend = renderer_get_backend(renderer);

	Context *context = ls_malloc(sizeof(Context));
	context->backend = backend;
	context->window = window;

	switch (backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			context->opengl3_context = opengl3_context_create(renderer_get_opengl3(renderer), window);
			LS_ASSERT(context->opengl3_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", backend);
	};

	return context;
}

void renderer_context_destroy(Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			opengl3_context_destroy(context->opengl3_context);
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
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			opengl3_context_make_current(context->opengl3_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

void renderer_context_swap_buffers(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			opengl3_context_swap_buffers(context->opengl3_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}