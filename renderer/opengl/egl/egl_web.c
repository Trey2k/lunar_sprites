#include "renderer/opengl/egl/egl.h"

#include "renderer/opengl/egl/extensions.h"

#include "core/core.h"
#include "core/log.h"

static EGLDisplay egl_display;
static EGLConfig egl_config;

bool egl_init(const OS *os) {
	LS_ASSERT(os);

	egl_display = eglGetDisplay(os_get_native_display(os));
	if (egl_display == EGL_NO_DISPLAY) {
		ls_log(LOG_LEVEL_WARNING, "Failed to get EGL display.\n");
		return false;
	}

	if (!eglInitialize(egl_display, NULL, NULL)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to initialize EGL.\n");
		return false;
	}

	String egl_version = eglQueryString(egl_display, EGL_VERSION);
	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %s.\n", egl_version);

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

	return true;
}

void egl_deinit() {
	eglTerminate(egl_display);
}

EGLDisplay egl_get_display() {
	return egl_display;
}

EGLConfig egl_get_config() {
	return egl_config;
}