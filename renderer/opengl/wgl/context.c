#include "renderer/opengl/wgl/context.h"

#include "core/debug.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "core/window.h"
#include "platform/window.h"
#include "renderer/opengl/context.h"

#include <glad/wgl.h>

static const int32 attributes[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	WGL_CONTEXT_MINOR_VERSION_ARB, 2,
	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES_PROFILE_BIT_EXT,
	0
};

OpenGLContext *opengl_context_create(const LSWindow *window) {
	LS_ASSERT(window);

	LSNativeWindow native_window = window_get_native_window(window);
	HDC device_context = GetDC(native_window);
	if (!device_context) {
		ls_log_fatal("Failed to get device context.\n");
		return NULL;
	}

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = ChoosePixelFormat(device_context, &pfd);

	int32 format = ChoosePixelFormat(device_context, &pfd);
	if (format == 0 || SetPixelFormat(device_context, format, &pfd) == false) {
		ReleaseDC(native_window, device_context);
		ls_log_fatal("Failed to set pixel format.\n");
		return NULL;
	}

	HGLRC temp_context = wglCreateContext(device_context);
	if (!temp_context) {
		ReleaseDC(native_window, device_context);
		ls_log_fatal("Failed to create the initial rendering context.\n");
		return NULL;
	}
	wglMakeCurrent(device_context, temp_context);

	gladLoaderLoadWGL(device_context);

	HGLRC opengl_context = wglCreateContextAttribsARB(device_context, NULL, attributes);
	if (!opengl_context) {
		opengl_context_make_current(NULL);
		wglDeleteContext(temp_context);
		ReleaseDC(native_window, device_context);
		ls_log_fatal("Failed to create the OpenGL context.\n");
		return NULL;
	}

	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));
	context->device_context = device_context;
	context->opengl_context = opengl_context;
	context->native_window = native_window;

	opengl_context_make_current(NULL);
	wglDeleteContext(temp_context);
	opengl_context_make_current(context);

	int32 version = gladLoaderLoadGLES2();
	if (!version) {
		opengl_context_destroy(context);
		ls_log_fatal("Failed to load OpenGL.\n");
		return NULL;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	ls_log(LOG_LEVEL_INFO, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	opengl_context_make_current(NULL);

	return context;
}

void opengl_context_destroy(OpenGLContext *context) {
	LS_ASSERT(context);

	opengl_context_make_current(NULL);
	wglDeleteContext(context->opengl_context);
	ReleaseDC(context->native_window, context->device_context);
	ls_free(context);
}

void opengl_context_make_current(const OpenGLContext *context) {
	if (!context) {
		wglMakeCurrent(NULL, NULL);
		return;
	}

	wglMakeCurrent(context->device_context, context->opengl_context);
}

void opengl_context_swap_buffers(const OpenGLContext *context) {
	LS_ASSERT(context);

	SwapBuffers(context->device_context);
}

void wgl_init(const OS *os) {
	// Nothing to do here
}

void wgl_deinit() {
	// Nothing to do here
}