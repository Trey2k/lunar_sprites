#ifndef WGL_RENDERER_H
#define WGL_RENDERER_H

#include "renderer/opengl/renderer.h"

#include <glad/wgl.h>

#include <windows.h>

struct OpenGLRenderer {
#if defined(MSVC_ENABLED)
	int32 dummy;
#endif // USE_MSVC
};

#endif // WGL_RENDERER_H