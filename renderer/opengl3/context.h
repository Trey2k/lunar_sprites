#ifndef OPENGL3_CONTEXT_H
#define OPENGL3_CONTEXT_H

#include "core/window/window.h"
#include "renderer/opengl3/renderer.h"
#include <glad/egl.h>
#include <glad/gl.h>

static const EGLint context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MINOR_VERSION_KHR, 3,
	EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
	EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
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
void opengl3_context_swap_buffers(const OpenGL3Context *context);

#endif // OPENGL3_CONTEXT_H