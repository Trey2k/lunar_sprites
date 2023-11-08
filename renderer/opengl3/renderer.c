#include "renderer/opengl3/renderer.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/os/os.h"

OpenGL3Renderer *opengl3_renderer_create(const OS *os) {
	int32 version = gladLoaderLoadEGL(NULL);
	if (!version) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load EGL.\n");
		return NULL;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on first load.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	EGLDisplay egl_display = eglGetDisplay(os_get_native_display(os));
	if (egl_display == EGL_NO_DISPLAY) {
		ls_log(LOG_LEVEL_ERROR, "Failed to get EGL display.\n");
		return NULL;
	}

	if (!eglInitialize(egl_display, NULL, NULL)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to initialize EGL.\n");
		return NULL;
	}

	version = gladLoaderLoadEGL(egl_display);
	if (!version) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load EGL.\n");
		return NULL;
	}

	EGLConfig egl_config;
	EGLint num_configs;
	if (!eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to choose config (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	if (num_configs != 1) {
		ls_log(LOG_LEVEL_ERROR, "Didn't get exactly one config, but %d\n", num_configs);
		return NULL;
	}

	if (!eglBindAPI(EGL_OPENGL_API)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to enable OpenGL API (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	OpenGL3Renderer *renderer = ls_malloc(sizeof(OpenGL3Renderer));
	renderer->egl_display = egl_display;
	renderer->egl_config = egl_config;

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on reload.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	return renderer;
}

void opengl3_renderer_destroy(OpenGL3Renderer *renderer) {
	LS_ASSERT(renderer);

	eglTerminate(renderer->egl_display);
	ls_free(renderer);
	gladLoaderUnloadEGL();
}