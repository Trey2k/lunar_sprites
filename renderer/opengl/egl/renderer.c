#include "renderer/opengl/egl/renderer.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/os/os.h"
#include "glad/egl.h"

static const EGLint attribs[] = {
	EGL_BUFFER_SIZE,
	32,
#if defined(OPENGL_MODE_GLES)
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES3_BIT,
#endif
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	EGL_NONE,
};

OpenGLRenderer *opengl_renderer_create(const OS *os) {
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

	EGLint renderable_type, surface_type;
	eglGetConfigAttrib(egl_display, egl_config, EGL_RENDERABLE_TYPE, &renderable_type);
	eglGetConfigAttrib(egl_display, egl_config, EGL_SURFACE_TYPE, &surface_type);

#if defined(OPENGL_MODE_GLES)
	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to bind OpenGL ES API (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}
#else
	if (!eglBindAPI(EGL_OPENGL_API)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to bind OpenGL API (eglError: 0x%X)\n", eglGetError());
		return NULL;
	}
#endif

	OpenGLRenderer *renderer = ls_malloc(sizeof(OpenGLRenderer));
	renderer->egl_display = egl_display;
	renderer->egl_config = egl_config;

	ls_log(LOG_LEVEL_INFO, "Loaded EGL version %d.%d on reload.\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	return renderer;
}

void opengl_renderer_destroy(OpenGLRenderer *renderer) {
	LS_ASSERT(renderer);

	eglTerminate(renderer->egl_display);
	ls_free(renderer);
	gladLoaderUnloadEGL();
}