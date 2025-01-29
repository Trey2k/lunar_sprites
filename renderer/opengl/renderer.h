#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "core/core.h"

#include "renderer/renderer_interface.h"

typedef struct OpenGLRenderer OpenGLRenderer;

OpenGLRenderer *opengl_renderer_create();
void opengl_renderer_start(OpenGLRenderer *renderer);
void opengl_renderer_destroy(OpenGLRenderer *renderer);

void opengl_register_methods(RendererInterface *renderer_interface);

#if defined(WGL_ENABLED)
bool opengl_wgl_enabled(const OpenGLRenderer *renderer);
#endif // WGL_ENABLED

#if defined(EGL_ENABLED)
bool opengl_egl_enabled(const OpenGLRenderer *renderer);
#endif // EGL_ENABLED

#endif // OPENGL_RENDERER_H