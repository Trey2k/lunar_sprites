#ifndef CONTEXT_H
#define CONTEXT_H

#include "core/window.h"
#include "renderer/renderer.h"

#if defined(GLES2_ENABLED)
#include "renderer/gles3/context.h"
#endif

typedef struct {
	const LSWindow *window;

	RendererBackend backend;
	union {
#if defined(GLES2_ENABLED)
		GLES2Context *gles3_context;
#endif
	};
} Context;

Context *renderer_context_create(const Renderer *renderer, const LSWindow *window);
void renderer_context_destroy(Context *context);

void renderer_context_make_current(const Context *context);
void renderer_context_swap_buffers(const Context *context);

#endif // CONTEXT_H