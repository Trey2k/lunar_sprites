#include "renderer/opengl/renderer_interface.h"

#include <glad/gl.h>

void opengl_set_clear_color(float32 r, float32 g, float32 b, float32 a) {
	glClearColor(r, g, b, a);
}

void opengl_clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}