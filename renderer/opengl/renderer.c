#include "renderer/opengl/renderer.h"

#include "core/flags.h"
#include "core/log.h"
#include "core/types/string.h"

#if defined(EGL_ENABLED)
#include "renderer/opengl/egl/context.h"
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
#include "renderer/opengl/wgl/context.h"
#endif // WGL_ENABLED

struct OpenGLRenderer {
#if defined(EGL_ENABLED)
	bool egl_enabled;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	bool wgl_enabled;
#endif // WGL_ENABLED
};

static struct OpenGLRenderer opengl;

void opengl_renderer_init() {
	// Flag registration would go here.
}

void opengl_renderer_start(const OS *os) {
#if defined(EGL_ENABLED)
	opengl.egl_enabled = egl_init(os);
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	opengl.wgl_enabled = wgl_init(os);
#endif // WGL_ENABLED
}

void opengl_renderer_deinit() {
#if defined(EGL_ENABLED)
	if (opengl.egl_enabled) {
		egl_deinit();
	}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	if (opengl.wgl_enabled) {
		wgl_deinit();
	}
#endif // WGL_ENABLED
}

#if defined(EGL_ENABLED)
bool opengl_egl_enabled() {
	return opengl.egl_enabled;
}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
bool opengl_wgl_enabled() {
	return opengl.wgl_enabled;
}
#endif // WGL_ENABLED