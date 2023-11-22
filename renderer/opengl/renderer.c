#include "renderer/opengl/renderer.h"
#include "renderer/opengl/renderer_interface.h"

#include "core/core.h"

#include <glad/gl.h>

#if defined(EGL_ENABLED)
#include "renderer/opengl/egl/context.h"
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
#include "renderer/opengl/wgl/context.h"
#endif // WGL_ENABLED

struct OpenGLRenderer {
	const LSCore *core;

#if defined(EGL_ENABLED)
	bool egl_enabled;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	bool wgl_enabled;
#endif // WGL_ENABLED
};

OpenGLRenderer *opengl_renderer_create(const LSCore *core) {
	OpenGLRenderer *renderer = ls_malloc(sizeof(OpenGLRenderer));
	renderer->core = core;

#if defined(EGL_ENABLED)
	renderer->egl_enabled = egl_init(core_get_os(core));
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	renderer->wgl_enabled = wgl_init(core_get_os(core));
#endif // WGL_ENABLED

	return renderer;
}

void opengl_renderer_destroy(OpenGLRenderer *renderer) {
#if defined(EGL_ENABLED)
	if (renderer->egl_enabled) {
		egl_deinit();
	}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	if (renderer->wgl_enabled) {
		wgl_deinit();
	}
#endif // WGL_ENABLED

	ls_free(renderer);
}

void opengl_register_methods(RendererInterface *renderer_interface) {
	renderer_interface->set_clear_color = opengl_set_clear_color;
	renderer_interface->clear = opengl_clear;
}

const LSCore *opengl_renderer_get_core(const OpenGLRenderer *renderer) {
	return renderer->core;
}

#if defined(EGL_ENABLED)
bool opengl_egl_enabled(const OpenGLRenderer *renderer) {
	return renderer->egl_enabled;
}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
bool opengl_wgl_enabled(const OpenGLRenderer *renderer) {
	return renderer->wgl_enabled;
}
#endif // WGL_ENABLED