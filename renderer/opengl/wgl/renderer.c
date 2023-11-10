#include "renderer/opengl/wgl/renderer.h"
#include "core/memory.h"
#include "core/os/os.h"

OpenGLRenderer *opengl_renderer_create(const OS *os) {
	OpenGLRenderer *renderer = ls_malloc(sizeof(OpenGLRenderer));

	return renderer;
}

void opengl_renderer_destroy(OpenGLRenderer *renderer) {
	ls_free(renderer);
}