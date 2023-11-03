#ifndef OPENGL3_RENDERER_H
#define OPENGL3_RENDERER_H

#include <EGL/egl.h>
#include <GL/gl.h>

#include "core/os/os_window.h"

typedef EGLSurface OpenGL3Surface;

typedef struct {
} OpenGL3Renderer;

OpenGL3Renderer *opengl3_renderer_create();
void opengl3_renderer_destroy(OpenGL3Renderer *renderer);

#endif // OPENGL3_RENDERER_H