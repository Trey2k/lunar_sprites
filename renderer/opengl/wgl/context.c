#include "renderer/opengl/wgl/context.h"

#include "core/debug.h"
#include "core/memory.h"
#include "core/types/typedefs.h"
#include "renderer/window.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/wgl/extensions.h"

#include "platform/window.h"
#include "platform/windows/os.h"
#include "platform/windows/window.h"

#include <glad/wgl.h>
#include <wingdi.h>

static bool choose_pixel_format(LSNativeWindow native_window, HDC device_context, PIXELFORMATDESCRIPTOR *pfd);
static uint32 load_wgl(const OpenGLRenderer *renderer, PIXELFORMATDESCRIPTOR *pfd);

LSWGLContext *wgl_context_create(const OpenGLRenderer *renderer, const LSWindow *window) {
	LS_ASSERT(window);
	LSNativeWindow native_window = window_get_native_window(window);
	HDC device_context = GetDC(native_window);
	if (!device_context) {
		ls_log(LOG_LEVEL_WARNING, "Failed to get device context.\n");
		return NULL;
	}

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;

	int32 err = load_wgl(renderer, &pfd);
	if (err != 0) {
		ReleaseDC(native_window, device_context);
		ls_log(LOG_LEVEL_WARNING, "Failed to load . %lu\n", GetLastError());
	}

	if (!choose_pixel_format(native_window, device_context, &pfd)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to set pixel format.\n");
	}

	Slice32 *attributes = slice32_create(10);
	slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_MAJOR_VERSION_ARB));
	slice32_append(attributes, SLICE_VAL32(i32, 3));

	if (wgl_has_extension(device_context, "WGL_EXT_create_context_es2_profile")) {
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_MINOR_VERSION_ARB));
		slice32_append(attributes, SLICE_VAL32(i32, 0));
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_PROFILE_MASK_ARB));
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_ES2_PROFILE_BIT_EXT));
	} else {
		ls_log(LOG_LEVEL_WARNING, "WGL_EXT_create_context_es2_profile is not supported. Using OpenGL 3.3\n");
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_MINOR_VERSION_ARB));
		slice32_append(attributes, SLICE_VAL32(i32, 3));
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_PROFILE_MASK_ARB));
		slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_CORE_PROFILE_BIT_ARB));
	}

	if (wgl_has_extension(device_context, "WGL_EXT_swap_control")) {
		wglSwapIntervalEXT(0);
	}

	slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_FLAGS_ARB));
	slice32_append(attributes, SLICE_VAL32(i32, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB));
	slice32_append(attributes, SLICE_VAL32(i32, 0));

	HGLRC opengl_context = wglCreateContextAttribsARB(device_context, NULL, slice32_get_data(attributes));
	if (!opengl_context) {
		ReleaseDC(native_window, device_context);
		ls_log(LOG_LEVEL_WARNING, "Failed to create the OpenGL context. 0x%X\n", GetLastError());
		return NULL;
	}

	slice32_destroy(attributes);

	LSWGLContext *context = ls_malloc(sizeof(LSWGLContext));
	context->device_context = device_context;
	context->opengl_context = opengl_context;
	context->native_window = native_window;

	wgl_context_make_current(context);

	int32 version = gladLoaderLoadGL();
	if (!version) {
		wgl_context_destroy(context);
		ls_log(LOG_LEVEL_WARNING, "Failed to load OpenGL.\n");
		return NULL;
	}

	ls_log(LOG_LEVEL_INFO, "Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	ls_log(LOG_LEVEL_INFO, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	ls_log(LOG_LEVEL_INFO, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	ls_log(LOG_LEVEL_INFO, "OpenGL version: %s\n", glGetString(GL_VERSION));
	ls_log(LOG_LEVEL_INFO, "OpenGL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	wgl_context_make_current(NULL);

	return context;
}

void wgl_context_destroy(LSWGLContext *context) {
	LS_ASSERT(context);

	wgl_context_make_current(NULL);
	wglDeleteContext(context->opengl_context);
	ReleaseDC(context->native_window, context->device_context);
	ls_free(context);
}

void wgl_context_make_current(const LSWGLContext *context) {
	if (!context) {
		wglMakeCurrent(NULL, NULL);
		return;
	}

	wglMakeCurrent(context->device_context, context->opengl_context);
}

void wgl_context_detach(const LSWGLContext *context) {
	LS_ASSERT(context);

	wglMakeCurrent(NULL, NULL);
}

void wgl_context_swap_buffers(const LSWGLContext *context) {
	LS_ASSERT(context);

	SwapBuffers(context->device_context);
}

bool wgl_init(const OS *os) {
	return true;
}

void wgl_deinit() {
	// Nothing to do here
}

static bool choose_pixel_format(LSNativeWindow native_window, HDC device_context, PIXELFORMATDESCRIPTOR *pfd) {
	int32 format = ChoosePixelFormat(device_context, pfd);
	if (format == 0 || SetPixelFormat(device_context, format, pfd) == false) {
		ReleaseDC(native_window, device_context);
		return false;
	}

	return true;
}

static const WindowConfig TEMP_WINDOW_CONFIG = {
	.title = "Temp",
	.size = { { 1, 1 } },
	.position = { { 0, 0 } },
	.hidden = true,
	.fullscreen = false,
};

static uint32 load_wgl(const OpenGLRenderer *renderer, PIXELFORMATDESCRIPTOR *pfd) {
	const OS *base_os = core_get_os(opengl_renderer_get_core(renderer));
	const PlatformOS *os = os_get_platform_os(base_os);

	// WGL requires us to create another window for the temporary context.
	// We use PlatformWindow and not LSWindow as LSWindow will trigger context creation recureivly.

	// Windows platform does not need the renderer reference.
	PlatformWindow *window = platform_create_window(os, TEMP_WINDOW_CONFIG, NULL);
	LSNativeWindow native_window = platform_window_get_native_window(window);
	HDC device_context = GetDC(native_window);

	if (!choose_pixel_format(native_window, device_context, pfd)) {
		return 69000;
	}

	HGLRC temp_context = wglCreateContext(device_context);
	if (!temp_context) {
		ReleaseDC(native_window, device_context);
		return GetLastError();
	}
	wglMakeCurrent(device_context, temp_context);

	if (!gladLoaderLoadWGL(device_context)) {
		return 69001;
	}

	wgl_context_make_current(NULL);
	wglDeleteContext(temp_context);
	ReleaseDC(native_window, device_context);

	platform_destroy_window(window);

	return 0;
}