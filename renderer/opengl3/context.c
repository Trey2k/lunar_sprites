#include "renderer/opengl3/context.h"
#include "core/debug.h"
#include "core/log/log.h"
#include "core/memory/memory.h"
#include "core/types/typedefs.h"
#include "core/window/window.h"
#include <glad/egl.h>
#include <glad/gl.h>

OpenGL3Context *opengl3_context_create(const OpenGL3Renderer *renderer, const LSWindow *window) {
	LS_ASSERT(renderer);
	LS_ASSERT(window);

	unsigned long native_window = window_get_native_window(window);
	EGLSurface egl_surface = eglCreateWindowSurface(renderer->egl_display, renderer->egl_config, native_window, NULL);
	if (egl_surface == EGL_NO_SURFACE) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create EGL surface (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	EGLContext egl_context = eglCreateContext(renderer->egl_display, renderer->egl_config, EGL_NO_CONTEXT, context_attribs);
	if (egl_context == EGL_NO_CONTEXT) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create EGL context (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	OpenGL3Context *context = ls_malloc(sizeof(OpenGL3Context));
	context->renderer = renderer;
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	opengl3_context_make_current(context);

	int32 version = gladLoaderLoadGL();
	if (!version) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load OpenGL.\n");
		return NULL;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	opengl3_context_make_current(NULL);

	return context;
}

void opengl3_context_destroy(OpenGL3Context *context) {
	LS_ASSERT(context);

	eglDestroyContext(context->renderer->egl_display, context->egl_context);
	eglDestroySurface(context->renderer->egl_display, context->egl_surface);
	ls_free(context);
}

void opengl3_context_make_current(const OpenGL3Context *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	if (!eglMakeCurrent(context->renderer->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void opengl3_context_swap_buffers(const OpenGL3Context *context) {
	LS_ASSERT(context);

	if (!eglSwapBuffers(context->renderer->egl_display, context->egl_surface)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}