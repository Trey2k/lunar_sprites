#ifndef EGL_DISPLAY_H
#define EGL_DISPLAY_H

#include "core/os.h"

// We don't use glad for EGL on web, because it's not needed.
#if !defined(WEB_ENABLED)
#include <glad/egl.h>
#else
#include <EGL/egl.h>
#endif

bool egl_init(const OS *os);

void egl_deinit();

EGLDisplay egl_get_display();
EGLConfig egl_get_config();

#endif // EGL_DISPLAY_H