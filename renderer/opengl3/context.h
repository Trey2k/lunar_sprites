#ifndef OPENGL3_CONTEXT_H
#define OPENGL3_CONTEXT_H

#include "core/os/window.h"
#include "renderer/opengl3/renderer.h"
#include <glad/egl.h>

static const EGLint context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION, 3,
	EGL_CONTEXT_MINOR_VERSION, 3,
	EGL_NONE
};

typedef struct {
	const OpenGL3Renderer *renderer;

	EGLSurface egl_surface;
	EGLContext egl_context;
} OpenGL3Context;

OpenGL3Context *opengl3_context_create(const OpenGL3Renderer *renderer, const LSWindow *window);
void opengl3_context_destroy(OpenGL3Context *context);

void opengl3_context_make_current(const OpenGL3Context *context);

#endif // OPENGL3_CONTEXT_H