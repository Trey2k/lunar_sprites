#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "core/os/os.h"

typedef struct OpenGLRenderer OpenGLRenderer;

OpenGLRenderer *opengl_renderer_create(const OS *os);
void opengl_renderer_destroy(OpenGLRenderer *renderer);

#endif // OPENGL_RENDERER_H