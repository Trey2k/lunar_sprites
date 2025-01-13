#include "renderer/opengl/egl/context.h"

#include "core/core.h"

#include "renderer/opengl/egl/egl.h"
#include "renderer/window.h"

#include <glad/gl.h>

LSEGLContext *egl_context_create(const LSWindow *window) {
	LS_ASSERT(window);

	EGLDisplay egl_display = egl_get_display();
	EGLConfig egl_config = egl_get_config();

	LSNativeWindow native_window = window_get_native_window(window);
	EGLSurface egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_window, NULL);
	if (egl_surface == EGL_NO_SURFACE) {
		ls_log(LOG_LEVEL_WARNING, "Failed to create EGL surface (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	Slice32 *context_attribs = slice32_create(5);
	slice32_append(context_attribs, SLICE_VAL32(i32, EGL_CONTEXT_CLIENT_VERSION));
	slice32_append(context_attribs, SLICE_VAL32(i32, 3));
	slice32_append(context_attribs, SLICE_VAL32(i32, EGL_NONE));

	EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, slice32_get_data(context_attribs));
	if (egl_context == EGL_NO_CONTEXT) {
		ls_log(LOG_LEVEL_WARNING, "Failed to create EGL context (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	slice32_destroy(context_attribs);

	LSEGLContext *context = ls_malloc(sizeof(LSEGLContext));
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	egl_context_make_current(context);

	int32 version = gladLoaderLoadGLES2();

	if (!version) {
		ls_log(LOG_LEVEL_WARNING, "Failed to load OpenGl.\n");
		return false;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL GLSL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	egl_context_make_current(NULL);

	return context;
}

void egl_context_destroy(LSEGLContext *context) {
	LS_ASSERT(context);

	EGLDisplay egl_display = egl_get_display();

	eglDestroyContext(egl_display, context->egl_context);
	eglDestroySurface(egl_display, context->egl_surface);
	ls_free(context);
}

void egl_context_make_current(const LSEGLContext *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	EGLDisplay egl_display = egl_get_display();

	if (!eglMakeCurrent(egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log_fatal("Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void egl_context_detach(const LSEGLContext *context) {
	LS_ASSERT(context);

	egl_context_make_current(NULL);
}

void egl_context_swap_buffers(const LSEGLContext *context) {
	LS_ASSERT(context);

	EGLDisplay egl_display = egl_get_display();

	if (!eglSwapBuffers(egl_display, context->egl_surface)) {
		ls_log_fatal("Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}
