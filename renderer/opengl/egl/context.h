#ifndef GLES3_CONTEXT_H
#define GLES3_CONTEXT_H

#include <glad/egl.h>

#include "renderer/opengl/context.h"
#include "renderer/opengl/renderer.h"

typedef struct {
	EGLSurface egl_surface;
	EGLContext egl_context;
} LSEGLContext;

bool egl_init(const OS *os);
void egl_deinit();

LSEGLContext *egl_context_create(const LSWindow *window);
void egl_context_destroy(LSEGLContext *context);

void egl_context_make_current(const LSEGLContext *context);
void egl_context_swap_buffers(const LSEGLContext *context);

#endif // GLES3_CONTEXT_H