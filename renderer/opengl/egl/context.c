#include "renderer/opengl/egl/context.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "renderer/window.h"

#include <glad/egl.h>
#include <glad/gl.h>

static const EGLint egl_attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES3_BIT,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

static const EGLint context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MINOR_VERSION_KHR, 0,
	EGL_NONE
};

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

	EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
	if (egl_context == EGL_NO_CONTEXT) {
		ls_log(LOG_LEVEL_WARNING, "Failed to create EGL context (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	LSEGLContext *context = ls_malloc(sizeof(LSEGLContext));
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	egl_context_make_current(context);

	int32 version = gladLoaderLoadGLES2();

	if (!version) {
		ls_log(LOG_LEVEL_WARNING, "Failed to load OpenGl ES.\n");
		return false;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL ES version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, "OpenGL ES vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL ES GLSL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

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

	EGLint num_configs;
	if (!eglChooseConfig(egl_display, egl_attribs, &egl_config, 1, &num_configs)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to choose config (eglError: 0x%X)\n", eglGetError());
		return false;
	}

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