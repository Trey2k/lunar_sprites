#ifndef WGL_CONTEXT_H
#define WGL_CONTEXT_H

#include "renderer/context.h"

#include <glad/wgl.h>
#include <windows.h>

struct OpenGLContext {
	HDC device_context;
	HGLRC opengl_context;
	LSNativeWindow native_window;
};

void wgl_init(const OS *os);
void wgl_deinit();

#endif // WGL_CONTEXT_H