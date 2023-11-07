#ifndef OPENGL3_RENDERER_H
#define OPENGL3_RENDERER_H

#include <glad/egl.h>
#include <glad/gl.h>

static const EGLint attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

typedef struct {
	EGLDisplay egl_display;
	EGLConfig egl_config;
} OpenGL3Renderer;

OpenGL3Renderer *opengl3_renderer_create();
void opengl3_renderer_destroy(OpenGL3Renderer *renderer);

#endif // OPENGL3_RENDERER_H