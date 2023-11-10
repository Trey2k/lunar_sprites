#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "core/os/os.h"

typedef struct OpenGLRenderer OpenGLRenderer;

void opengl_renderer_init();
void opengl_renderer_start(const OS *os);
void opengl_renderer_deinit();

#endif // OPENGL_RENDERER_H