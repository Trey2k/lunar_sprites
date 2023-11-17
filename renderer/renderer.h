#ifndef RENDERER_H
#define RENDERER_H

#include "core/api.h"
#include "core/flags.h"
#include "core/os/os.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

#define RENDERER_BACKEND_COUNT 2

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL,
} RendererBackend;

typedef struct {
	const OS *os;

	FlagValue *backend_flag;

	RendererBackend backend;
} Renderer;

void renderer_init();
void renderer_start(const OS *os);
void renderer_deinit();

LS_EXPORT void renderer_set_clear_color(float32 r, float32 g, float32 b, float32 a);
void renderer_clear();

RendererBackend renderer_get_backend();

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