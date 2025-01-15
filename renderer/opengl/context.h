#ifndef OPENGL3_CONTEXT_H
#define OPENGL3_CONTEXT_H

#include "renderer/opengl/renderer.h"
#include "renderer/window.h"

typedef struct OpenGLContext OpenGLContext;

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, const LSWindow *window);
void opengl_context_destroy(OpenGLContext *context);

void opengl_context_make_current(const OpenGLContext *context);
void opengl_context_detach(const OpenGLContext *context);
void opengl_context_swap_buffers(OpenGLContext *context);

void opengl_context_resize(OpenGLContext *context, Vector2u size);

const LSWindow *opengl_context_get_window(const OpenGLContext *context);

#endif // OPENGL3_CONTEXT_H