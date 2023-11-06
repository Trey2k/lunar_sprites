#ifndef RENDERER_H
#define RENDERER_H

#include "core/typedefs.h"
#include "core/types/string.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/renderer.h"
typedef void *(*OPENGLloadproc)(const char *name);

extern const OPENGLloadproc OPENGL3_LOAD_PROC;

#endif

#define RENDERER_BACKEND_COUNT 2

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL3,
} RendererBackend;

_FORCE_INLINE_ String renderer_backend_to_string(RendererBackend backend) {
	switch (backend) {
		case RENDERER_BACKEND_NONE:
			return "NONE";
		case RENDERER_BACKEND_OPENGL3:
			return "OPENGL3";
		default:
			return "UNKNOWN";
	}
}

bool renderer_init(RendererBackend backend);
void renderer_deinit();

RendererBackend renderer_get_backend();

#if defined(OPENGL3_ENABLED)
const OpenGL3Renderer *renderer_get_opengl3();
#endif

#endif // RENDERER_H