#include "renderer/gles3/context.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "core/window.h"
#include <glad/egl.h>
#include <glad/gles2.h>

GLES2Context *gles3_context_create(const GLES2Renderer *renderer, const LSWindow *window) {
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

	GLES2Context *context = ls_malloc(sizeof(GLES2Context));
	context->renderer = renderer;
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	gles3_context_make_current(context);

	int32 version = gladLoaderLoadGLES2();
	if (!version) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load OpenGL ES.\n");
		return NULL;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL ES version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, "OpenGL ES vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	gles3_context_make_current(NULL);

	return context;
}

void gles3_context_destroy(GLES2Context *context) {
	LS_ASSERT(context);

	eglDestroyContext(context->renderer->egl_display, context->egl_context);
	eglDestroySurface(context->renderer->egl_display, context->egl_surface);
	ls_free(context);
}

void gles3_context_make_current(const GLES2Context *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	if (!eglMakeCurrent(context->renderer->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void gles3_context_swap_buffers(const GLES2Context *context) {
	LS_ASSERT(context);

	if (!eglSwapBuffers(context->renderer->egl_display, context->egl_surface)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}