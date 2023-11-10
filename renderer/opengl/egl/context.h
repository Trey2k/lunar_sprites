#ifndef GLES3_CONTEXT_H
#define GLES3_CONTEXT_H

#include <glad/egl.h>

#include "renderer/opengl/context.h"
#include "renderer/opengl/renderer.h"

struct OpenGLContext {
	EGLSurface egl_surface;
	EGLContext egl_context;
};

typedef enum {
	LS_OPENGL_API_GL,
	LS_OPENGL_API_GLES,
} OpenGLAPI;

void egl_init(const OS *os, OpenGLAPI api);
void egl_deinit();

#endif // GLES3_CONTEXT_H