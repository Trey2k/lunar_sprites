#include "renderer/opengl3/context.h"
#include "core/core.h"
#include "core/os/window.h"
#include "core/typedefs.h"
#include <glad/egl.h>

OpenGL3Context *opengl3_context_create(const OpenGL3Renderer *renderer, const LSWindow *window) {
	CORE_ASSERT(renderer);
	CORE_ASSERT(window);

	unsigned long native_window = window_get_native_window(window);

	EGLSurface egl_surface = eglCreateWindowSurface(renderer->egl_display, renderer->egl_config, native_window, NULL);
	if (egl_surface == EGL_NO_SURFACE) {
		core_log(LOG_LEVEL_ERROR, "Failed to create EGL surface (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	EGLContext egl_context = eglCreateContext(renderer->egl_display, renderer->egl_config, EGL_NO_CONTEXT, context_attribs);
	if (egl_context == EGL_NO_CONTEXT) {
		core_log(LOG_LEVEL_ERROR, "Failed to create EGL context (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	OpenGL3Context *context = core_malloc(sizeof(OpenGL3Context));
	context->renderer = renderer;
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	return context;
}

void opengl3_context_destroy(OpenGL3Context *context) {
	CORE_ASSERT(context);

	eglDestroyContext(context->renderer->egl_display, context->egl_context);
	eglDestroySurface(context->renderer->egl_display, context->egl_surface);
	core_free(context);
}

void opengl3_context_make_current(const OpenGL3Context *context) {
	CORE_ASSERT(context);

	if (!eglMakeCurrent(context->renderer->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		core_log(LOG_LEVEL_ERROR, "Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}