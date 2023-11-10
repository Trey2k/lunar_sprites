#ifndef GLES3_CONTEXT_H
#define GLES3_CONTEXT_H

#include <glad/egl.h>

#include "renderer/opengl/context.h"
#include "renderer/opengl/egl/renderer.h"
#include "renderer/opengl/renderer.h"

struct OpenGLContext {
	const OpenGLRenderer *renderer;

	EGLSurface egl_surface;
	EGLContext egl_context;
};

#endif // GLES3_CONTEXT_H