#ifndef GLES3_RENDERER_H
#define GLES3_RENDERER_H

#include "renderer/opengl/renderer.h"

#include <glad/egl.h>

struct OpenGLRenderer {
	EGLDisplay egl_display;
	EGLConfig egl_config;
};

#endif // GLES3_RENDERER_H