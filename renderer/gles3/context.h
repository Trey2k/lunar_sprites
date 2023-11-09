#ifndef OPENGL3_CONTEXT_H
#define OPENGL3_CONTEXT_H

#include "core/window.h"
#include "renderer/gles3/renderer.h"
#include <glad/egl.h>
#include <glad/gles2.h>

static const EGLint context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MAJOR_VERSION_KHR, 2,
	EGL_NONE
};

typedef struct {
	const GLES2Renderer *renderer;

	EGLSurface egl_surface;
	EGLContext egl_context;
} GLES2Context;

GLES2Context *gles3_context_create(const GLES2Renderer *renderer, const LSWindow *window);
void gles3_context_destroy(GLES2Context *context);

void gles3_context_make_current(const GLES2Context *context);
void gles3_context_swap_buffers(const GLES2Context *context);

#endif // OPENGL3_CONTEXT_H