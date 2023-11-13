#include "renderer/opengl/egl/context.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "renderer/opengl/egl/extensions.h"
#include "renderer/window.h"

#include <glad/egl.h>
#include <glad/gl.h>

static EGLDisplay egl_display;
static EGLConfig egl_config;

LSEGLContext *egl_context_create(const LSWindow *window) {
	LS_ASSERT(window);

	LSNativeWindow native_window = window_get_native_window(window);
	EGLSurface egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_window, NULL);
	if (egl_surface == EGL_NO_SURFACE) {
		ls_log(LOG_LEVEL_WARNING, "Failed to create EGL surface (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	Slice32 *context_attribs = slice32_create(5);
	slice32_append(context_attribs, SLICE_VAL32(i32, EGL_CONTEXT_MAJOR_VERSION_KHR));
	slice32_append(context_attribs, SLICE_VAL32(i32, 3));
	slice32_append(context_attribs, SLICE_VAL32(i32, EGL_CONTEXT_MINOR_VERSION_KHR));
	slice32_append(context_attribs, SLICE_VAL32(i32, 2));
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

	if (!version ||
			GLAD_VERSION_MAJOR(version) < 3 ||
			(GLAD_VERSION_MAJOR(version) == 3 && GLAD_VERSION_MINOR(version) < 2)) {
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

	eglDestroyContext(egl_display, context->egl_context);
	eglDestroySurface(egl_display, context->egl_surface);
	ls_free(context);
}

void egl_context_make_current(const LSEGLContext *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	if (!eglMakeCurrent(egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log_fatal("Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void egl_context_swap_buffers(const LSEGLContext *context) {
	LS_ASSERT(context);

	if (!eglSwapBuffers(egl_display, context->egl_surface)) {
		ls_log_fatal("Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}

bool egl_init(const OS *os) {
	LS_ASSERT(os);

	int32 version = gladLoaderLoadEGL(NULL);
	if (!version) {
		ls_log(LOG_LEVEL_WARNING, "Failed to load EGL.\n");
		return false;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on first load.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	egl_display = eglGetDisplay(os_get_native_display(os));
	if (egl_display == EGL_NO_DISPLAY) {
		ls_log(LOG_LEVEL_WARNING, "Failed to get EGL display.\n");
		return false;
	}

	if (!eglInitialize(egl_display, NULL, NULL)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to initialize EGL.\n");
		return false;
	}

	version = gladLoaderLoadEGL(egl_display);
	if (!version) {
		ls_log(LOG_LEVEL_WARNING, "Failed to load EGL.\n");
		return false;
	}

	if (!egl_has_extension(egl_display, "EGL_KHR_create_context")) {
		ls_log(LOG_LEVEL_WARNING, "EGL_KHR_create_context is not supported.\n");
		return false;
	}

	Slice32 *egl_attribs = slice32_create(16);
	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_BUFFER_SIZE));
	slice32_append(egl_attribs, SLICE_VAL32(i32, 32));

	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_RENDERABLE_TYPE));
	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_OPENGL_ES3_BIT));
	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_SURFACE_TYPE));
	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_WINDOW_BIT | EGL_PBUFFER_BIT));
	slice32_append(egl_attribs, SLICE_VAL32(i32, EGL_NONE));

	EGLint num_configs;
	if (!eglChooseConfig(egl_display, slice32_get_data(egl_attribs), &egl_config, 1, &num_configs)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to choose config (eglError: 0x%X)\n", eglGetError());
		return false;
	}

	slice32_destroy(egl_attribs);

	if (num_configs != 1) {
		ls_log(LOG_LEVEL_WARNING, "Didn't get exactly one config, but %d\n", num_configs);
		return false;
	}

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to bind OpenGL ES API (eglError: 0x%X)\n", eglGetError());
		return false;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on reload.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	return true;
}

void egl_deinit() {
	eglTerminate(egl_display);
}