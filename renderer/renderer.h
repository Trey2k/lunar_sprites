#ifndef RENDERER_H
#define RENDERER_H

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL,
} RendererBackend;

typedef struct Context Context;

void renderer_init(LSCore *core);
void renderer_start();
void renderer_deinit();

void renderer_set_active_window(const LSWindow *window);
void renderer_set_active_context(const Context *context);

LSCore *renderer_get_core();

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl();
#endif

// Sets the clear color for the renderer. There must be a valid rendering context.
LS_EXPORT void renderer_set_clear_color(float32 r, float32 g, float32 b, float32 a);
// Clears the screen with the clear color.
LS_EXPORT void renderer_clear();
// Returns the backend used by the renderer.
LS_EXPORT RendererBackend renderer_get_backend();

LS_EXPORT Vector2u renderer_get_viewport_size();

_FORCE_INLINE_ String renderer_backend_to_string(RendererBackend backend) {
	switch (backend) {
		case RENDERER_BACKEND_NONE:
			return "NONE";
		case RENDERER_BACKEND_OPENGL:
			return "OPENGL3";
		default:
			return "UNKNOWN";
	}
}

#endif // RENDERER_H