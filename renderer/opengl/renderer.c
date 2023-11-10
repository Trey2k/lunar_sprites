#include "renderer/opengl/renderer.h"

#include "core/flags.h"
#include "core/log.h"
#include "core/types/string.h"

#if defined(EGL_ENABLED)
#include "renderer/opengl/egl/context.h"
#elif defined(WGL_ENABLED)
#include "renderer/opengl/wgl/context.h"
#endif // EGL_ENABLED

#if defined(EGL_ENABLED)

struct OpenGLRenderer {
	FlagValue *api_flag;
	OpenGLAPI api;
};

static struct OpenGLRenderer opengl;

static void check_flags();

#endif // EGL_ENABLED

void opengl_renderer_init() {
#if defined(EGL_ENABLED)
	opengl.api_flag = ls_register_flag("opengl-api", FLAG_TYPE_STRING, (FlagValue){ .string = "GL" },
			"The OpenGL API to use. Valid values are GL and GLES.");
#endif // WINDOWS_ENABLED
}

void opengl_renderer_start(const OS *os) {
#if defined(EGL_ENABLED)
	check_flags();
	egl_init(os, opengl.api);
#elif defined(WGL_ENABLED)
	wgl_init(os);
#endif // EGL_ENABLED
}

void opengl_renderer_deinit() {
#if defined(EGL_ENABLED)
	egl_deinit();
#elif defined(WGL_ENABLED)
	wgl_deinit();
#endif // EGL_ENABLED
}

#if defined(EGL_ENABLED)

static void check_flags() {
	ls_str_to_upper(opengl.api_flag->string);

	if (ls_str_equals(opengl.api_flag->string, "GL")) {
		opengl.api = LS_OPENGL_API_GL;
	} else if (ls_str_equals(opengl.api_flag->string, "GLES")) {
		opengl.api = LS_OPENGL_API_GLES;
	} else {
		ls_log_fatal("Invalid OpenGL API: %s\n", opengl.api_flag->string);
	}
}

#endif // EGL_ENABLED