#include "renderer/opengl/egl/context.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "core/window.h"

#include <glad/egl.h>

#if defined(OPENGL_MODE_GLES)
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif

static const EGLint context_attribs[] = {
	EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
	EGL_CONTEXT_MINOR_VERSION_KHR, 2,
#if defined(OPENGL_MODE_GL)
	EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
	EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
#endif
	EGL_NONE
};

#if defined(OPENGL_MODE_GLES)
#define OPENGL_NAME "OpenGL ES"
#else
#define OPENGL_NAME "OpenGL"
#endif

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, const LSWindow *window) {
	LS_ASSERT(renderer);
	LS_ASSERT(window);

	LSNativeWindow native_window = window_get_native_window(window);
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

	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));
	context->renderer = renderer;
	context->egl_surface = egl_surface;
	context->egl_context = egl_context;

	opengl_context_make_current(context);

#if defined(OPENGL_MODE_GLES)
	int32 version = gladLoaderLoadGLES2();
	if (!version) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load OpenGL ES.\n");
		return NULL;
	}
#else
	int32 version = gladLoaderLoadGL();
	if (!version) {
		opengl_context_destroy(context);
		ls_log(LOG_LEVEL_ERROR, "Failed to load OpenGL.\n");
		return NULL;
	}
#endif

	ls_log(LOG_LEVEL_INFO, "Loaded " OPENGL_NAME " version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	ls_log(LOG_LEVEL_INFO, OPENGL_NAME " vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, OPENGL_NAME " renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, OPENGL_NAME " version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, OPENGL_NAME " shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	opengl_context_make_current(NULL);

	return context;
}

void opengl_context_destroy(OpenGLContext *context) {
	LS_ASSERT(context);

	eglDestroyContext(context->renderer->egl_display, context->egl_context);
	eglDestroySurface(context->renderer->egl_display, context->egl_surface);
	ls_free(context);
}

void opengl_context_make_current(const OpenGLContext *context) {
	if (!context) {
		eglMakeCurrent(NULL, NULL, NULL, NULL);
		return;
	}

	if (!eglMakeCurrent(context->renderer->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to make EGL context current (eglError: 0x%X)\n", eglGetError());
	}
}

void opengl_context_swap_buffers(const OpenGLContext *context) {
	LS_ASSERT(context);

	if (!eglSwapBuffers(context->renderer->egl_display, context->egl_surface)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to swap EGL buffers (eglError: 0x%X)\n", eglGetError());
	}
}