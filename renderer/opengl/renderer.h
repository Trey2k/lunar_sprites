#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "core/os/os.h"

void opengl_renderer_init();
void opengl_renderer_start(const OS *os);
void opengl_renderer_deinit();

#if defined(WGL_ENABLED)
bool opengl_wgl_enabled();
#endif // WGL_ENABLED

#if defined(EGL_ENABLED)
bool opengl_egl_enabled();
#endif // EGL_ENABLED

#endif // OPENGL_RENDERER_H