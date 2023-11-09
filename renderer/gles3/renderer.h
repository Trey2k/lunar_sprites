#ifndef OPENGL3_RENDERER_H
#define OPENGL3_RENDERER_H

#include "core/os/os.h"
#include <glad/egl.h>
#include <glad/gles2.h>

static const EGLint attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES3_BIT,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

typedef struct {
	EGLDisplay egl_display;
	EGLConfig egl_config;
} GLES2Renderer;

GLES2Renderer *gles3_renderer_create(const OS *os);
void gles3_renderer_destroy(GLES2Renderer *renderer);

#endif // OPENGL3_RENDERER_H