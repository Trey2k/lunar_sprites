#include "renderer/opengl3/renderer.h"
#include "core/core.h"
#include "core/core_log.h"
#include "core/os/os.h"

OpenGL3Renderer *opengl3_renderer_create() {
	int32 version = gladLoaderLoadEGL(NULL);
	if (!version) {
		core_log(LOG_LEVEL_ERROR, "Failed to load EGL.\n");
		return NULL;
	}

	core_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on first load.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	EGLDisplay egl_display = eglGetDisplay(os_get_native_display());
	if (egl_display == EGL_NO_DISPLAY) {
		core_log(LOG_LEVEL_ERROR, "Failed to get EGL display.\n");
		return NULL;
	}

	if (!eglInitialize(egl_display, NULL, NULL)) {
		core_log(LOG_LEVEL_ERROR, "Failed to initialize EGL.\n");
		return NULL;
	}

	version = gladLoaderLoadEGL(egl_display);
	if (!version) {
		core_log(LOG_LEVEL_ERROR, "Failed to load EGL.\n");
		return NULL;
	}

	EGLConfig egl_config;
	EGLint num_configs;
	if (!eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs)) {
		core_log(LOG_LEVEL_ERROR, "Failed to choose config (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}

	if (num_configs != 1) {
		core_log(LOG_LEVEL_ERROR, "Didn't get exactly one config, but %d\n", num_configs);
		return NULL;
	}

	OpenGL3Renderer *renderer = core_malloc(sizeof(OpenGL3Renderer));
	renderer->egl_display = egl_display;
	renderer->egl_config = egl_config;

	core_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on reload.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	return renderer;
}

void opengl3_renderer_destroy(OpenGL3Renderer *renderer) {
	CORE_ASSERT(renderer);

	eglTerminate(renderer->egl_display);
	core_free(renderer);
	gladLoaderUnloadEGL();
}