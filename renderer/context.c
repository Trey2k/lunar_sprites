#include "renderer/context.h"
#include "core/debug.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "renderer/renderer.h"

#if defined(GLES2_ENABLED)
#include "renderer/gles3/context.h"
#endif

Context *renderer_context_create(const Renderer *renderer, const LSWindow *window) {
	const RendererBackend backend = renderer_get_backend(renderer);

	Context *context = ls_malloc(sizeof(Context));
	context->backend = backend;
	context->window = window;

	switch (backend) {
#if defined(GLES2_ENABLED)
		case RENDERER_BACKEND_GLES2: {
			context->gles3_context = gles3_context_create(renderer_get_gles3(renderer), window);
			LS_ASSERT(context->gles3_context);
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
#if defined(GLES2_ENABLED)
		case RENDERER_BACKEND_GLES2: {
			gles3_context_destroy(context->gles3_context);
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
#if defined(GLES2_ENABLED)
		case RENDERER_BACKEND_GLES2: {
			gles3_context_make_current(context->gles3_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}

void renderer_context_swap_buffers(const Context *context) {
	LS_ASSERT(context);

	switch (context->backend) {
#if defined(GLES2_ENABLED)
		case RENDERER_BACKEND_GLES2: {
			gles3_context_swap_buffers(context->gles3_context);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}