#ifndef OPENGL3_CONTEXT_H
#define OPENGL3_CONTEXT_H

#include "core/window.h"
#include "renderer/opengl/renderer.h"

typedef struct OpenGLContext OpenGLContext;

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, const LSWindow *window);
void opengl_context_destroy(OpenGLContext *context);

void opengl_context_make_current(const OpenGLContext *context);
void opengl_context_swap_buffers(const OpenGLContext *context);

#endif // OPENGL3_CONTEXT_H