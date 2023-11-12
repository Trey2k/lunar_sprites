#ifndef CONTEXT_H
#define CONTEXT_H

#include "renderer/renderer.h"
#include "renderer/window.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/context.h"
#endif

typedef struct {
	const LSWindow *window;

	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLContext *opengl_context;
#endif
	};
} Context;

Context *renderer_context_create(const LSWindow *window);
void renderer_context_destroy(Context *context);

void renderer_context_make_current(const Context *context);
void renderer_context_swap_buffers(const Context *context);

#endif // CONTEXT_H