#ifndef EGL_CONTEXT_H
#define EGL_CONTEXT_H

#include "renderer/opengl/egl/egl.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/renderer.h"

typedef struct {
	EGLSurface egl_surface;
	EGLContext egl_context;
} LSEGLContext;

LSEGLContext *egl_context_create(const LSWindow *window);
void egl_context_destroy(LSEGLContext *context);

void egl_context_make_current(const LSEGLContext *context);
void egl_context_detach(const LSEGLContext *context);
void egl_context_swap_buffers(const LSEGLContext *context);

#endif // EGL_CONTEXT_H