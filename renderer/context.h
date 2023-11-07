#ifndef CONTEXT_H
#define CONTEXT_H

#include "core/os/window.h"
#include "renderer/renderer.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/context.h"
#endif

typedef struct {
	const LSWindow *window;

	RendererBackend backend;
	union {
#if defined(OPENGL3_ENABLED)
		OpenGL3Context *opengl3_context;
#endif
	};
} Context;

Context *context_create(const LSWindow *window);
void context_destroy(Context *context);

void context_make_current(const Context *context);
void context_swap_buffers(const Context *context);

#endif // CONTEXT_H