#include "opengl3_renderer.h"
#include "core/core.h"

OpenGL3Renderer *opengl3_renderer_create() {
	OpenGL3Renderer *renderer = core_malloc(sizeof(OpenGL3Renderer));
	return renderer;
}

void opengl3_renderer_destroy(OpenGL3Renderer *renderer) {
	core_free(renderer);
}