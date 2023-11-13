#ifndef WGL_CONTEXT_H
#define WGL_CONTEXT_H

#include "renderer/context.h"

#include <glad/wgl.h>
#include <windows.h>

typedef struct {
	HDC device_context;
	HGLRC opengl_context;
	LSNativeWindow native_window;
} LSWGLContext;

bool wgl_init(const OS *os);
void wgl_deinit();

LSWGLContext *wgl_context_create(const LSWindow *window);
void wgl_context_destroy(LSWGLContext *context);

void wgl_context_make_current(const LSWGLContext *context);
void wgl_context_swap_buffers(const LSWGLContext *context);

#endif // WGL_CONTEXT_H