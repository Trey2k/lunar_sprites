#include "renderer/opengl/egl/context.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "core/window.h"

#include <glad/egl.h>
#include <glad/gl.h>

static const EGLint gles_egl_attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES3_BIT,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

static const EGLint gles_context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MINOR_VERSION_KHR, 2,
	EGL_NONE
};

static const EGLint gl_egl_attribs[] = {
	EGL_BUFFER_SIZE,
	32,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

static const EGLint gl_context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MINOR_VERSION_KHR, 2,
	EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
	EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
	EGL_NONE
};

static String api_name = "OpenGL";
EGLenum egl_api = EGL_OPENGL_API;

static const EGLint *egl_attribs = gl_egl_attribs;
static const EGLint *context_attribs = gl_context_attribs;

static EGLDisplay egl_display;
static EGLConfig egl_config;

OpenGLContext *opengl_context_create(const LSWindow *window) {
	LS_ASSERT(window);

	LSNativeWindow native_window = window_get_native_window(window);
	EGLSurface egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_window, NULL);
	if (egl_surface == EGL_NO_SURFACE) {
		ls_log_fatal("Failed to create EGL surface (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
	if (egl_context == EGL_NO_CONTEXT) {
		ls_log_fatal("Failed to create EGL context (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	opengl_context_make_current(context);

	int32 version = 0;
	if (egl_api == EGL_OPENGL_ES_API) {
		version = gladLoaderLoadGLES2();
	}

	if (!version) {
		ls_log_fatal("Failed to load %s.\n", api_name);
	}

	ls_log(LOG_LEVEL_INFO, "Loaded %s version %d.%d\n", api_name, GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, "%s vendor: %s\n", api_name, glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "%s renderer: %s\n", api_name, glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "%s version: %s\n", api_name, glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "%s GLSL shading language version: %s\n", api_name, glGetString(GL_SHADING_LANGUAGE_VERSION));

	opengl_context_make_current(NULL);

	return context;
}

void opengl_context_destroy(OpenGLContext *context) {
	LS_ASSERT(context);

	eglDestroyContext(egl_display, context->egl_context);
	eglDestroySurface(egl_display, context->egl_surface);
	ls_free(context);
}

void opengl_context_make_current(const OpenGLContext *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	if (!eglMakeCurrent(egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log_fatal("Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void opengl_context_swap_buffers(const OpenGLContext *context) {
	LS_ASSERT(context);

	if (!eglSwapBuffers(egl_display, context->egl_surface)) {
		ls_log_fatal("Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}

void egl_init(const OS *os, OpenGLAPI api) {
	LS_ASSERT(os);

	switch (api) {
		case LS_OPENGL_API_GLES:
			egl_attribs = gles_egl_attribs;
			context_attribs = gles_context_attribs;
			api_name = "OpenGL ES";
			egl_api = EGL_OPENGL_ES_API;
			break;
		case LS_OPENGL_API_GL:
			egl_attribs = gl_egl_attribs;
			context_attribs = gl_context_attribs;
			api_name = "OpenGL";
			egl_api = EGL_OPENGL_API;
			break;
	};

	int32 version = gladLoaderLoadEGL(NULL);
	if (!version) {
		ls_log_fatal("Failed to load EGL.\n");
	}

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on first load.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	egl_display = eglGetDisplay(os_get_native_display(os));
	if (egl_display == EGL_NO_DISPLAY) {
		ls_log_fatal("Failed to get EGL display.\n");
	}

	if (!eglInitialize(egl_display, NULL, NULL)) {
		ls_log_fatal("Failed to initialize EGL.\n");
	}

	version = gladLoaderLoadEGL(egl_display);
	if (!version) {
		ls_log_fatal("Failed to load EGL.\n");
	}

	EGLint num_configs;
	if (!eglChooseConfig(egl_display, egl_attribs, &egl_config, 1, &num_configs)) {
		ls_log_fatal("Failed to choose config (eglError: 0x%X)\n", eglGetError());
	}

	if (num_configs != 1) {
		ls_log_fatal("Didn't get exactly one config, but %d\n", num_configs);
	}

	if (!eglBindAPI(egl_api)) {
		ls_log_fatal("Failed to bind %s API (eglError: 0x%X)\n", api_name, eglGetError());
	}

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on reload.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void egl_deinit() {
	eglTerminate(egl_display);
}