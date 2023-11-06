#ifndef OPENGL3_RENDERER_H
#define OPENGL3_RENDERER_H

#include <glad/egl.h>

static const EGLint attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_RED_SIZE,
	8,
	EGL_GREEN_SIZE,
	8,
	EGL_BLUE_SIZE,
	8,
	EGL_ALPHA_SIZE,
	8,

	EGL_DEPTH_SIZE,
	EGL_DONT_CARE,
	EGL_STENCIL_SIZE,
	EGL_DONT_CARE,

	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_BIT,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
	EGL_NONE,
};

typedef struct {
	EGLDisplay egl_display;
	EGLConfig egl_config;
} OpenGL3Renderer;

OpenGL3Renderer *opengl3_renderer_create();
void opengl3_renderer_destroy(OpenGL3Renderer *renderer);

#endif // OPENGL3_RENDERER_H