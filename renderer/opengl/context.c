#include "renderer/opengl/context.h"

#include "core/core.h"

#include "renderer/opengl/debug.h"
#include <GL/gl.h>

#if defined(EGL_ENABLED)
#include "renderer/opengl/egl/context.h"
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
#include "renderer/opengl/wgl/context.h"
#endif // WGL_ENABLED

#include <glad/gl.h>

typedef enum {
	INVALID_CONTEXT,
	EGL_CONTEXT,
	WGL_CONTEXT,
} ContextType;

struct OpenGLContext {
	ContextType type;
	LSWindow *window;

	LSMutex *viewport_mutex;
	Vector2u viewport_size;
	Vector2u future_size;
	bool should_resize;

	union {
#if defined(EGL_ENABLED)
		LSEGLContext *egl;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		LSWGLContext *wgl;
#endif // WGL_ENABLED
	};
};

static void opengl_init(OpenGLContext *context, const LSWindow *window) {
	GL_CALL(glEnable(GL_BLEND));
	GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	opengl_context_resize(context, window_get_size(window));
}

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, LSWindow *window) {
	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));
	context->window = window;
	context->viewport_mutex = os_mutex_create();
	context->should_resize = false;
#if defined(EGL_ENABLED)
	if (opengl_egl_enabled(renderer)) {
		context->egl = egl_context_create(window);
		if (context->egl) {
			context->type = EGL_CONTEXT;
			opengl_context_make_current(context);
			opengl_context_swap_buffers(context);
			opengl_init(context, window);
			return context;
		}
	}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	if (opengl_wgl_enabled(renderer)) {
		context->wgl = wgl_context_create(renderer, window);
		if (context->wgl) {
			context->type = WGL_CONTEXT;
			opengl_context_make_current(context);
			opengl_context_swap_buffers(context);
			opengl_init(context, window);
			return context;
		}
	}
#endif // WGL_ENABLED

	ls_free(context);
	return NULL;
}

void opengl_context_destroy(OpenGLContext *context) {
	LS_ASSERT(context);

	switch (context->type) {
#if defined(EGL_ENABLED)
		case EGL_CONTEXT: {
			egl_context_destroy(context->egl);
		} break;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		case WGL_CONTEXT: {
			wgl_context_destroy(context->wgl);
		} break;
#endif // WGL_ENABLED

		default:
			ls_log_fatal("Unknown OpenGL context type: %d\n", context->type);
	};

	ls_free(context);
}

void opengl_context_make_current(const OpenGLContext *context) {
	LS_ASSERT(context);

	switch (context->type) {
#if defined(EGL_ENABLED)
		case EGL_CONTEXT: {
			egl_context_make_current(context->egl);
		} break;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		case WGL_CONTEXT: {
			wgl_context_make_current(context->wgl);
		} break;
#endif // WGL_ENABLED

		default:
			ls_log_fatal("Unknown OpenGL context type: %d\n", context->type);
	};
}

void opengl_context_detach(const OpenGLContext *context) {
	LS_ASSERT(context);

	switch (context->type) {
#if defined(EGL_ENABLED)
		case EGL_CONTEXT: {
			egl_context_detach(context->egl);
		} break;

#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		case WGL_CONTEXT: {
			wgl_context_detach(context->wgl);
		} break;
#endif // WGL_ENABLED

		default:
			ls_log_fatal("Unknown OpenGL context type: %d\n", context->type);
	};
}

void opengl_context_swap_buffers(OpenGLContext *context) {
	LS_ASSERT(context);

	switch (context->type) {
#if defined(EGL_ENABLED)
		case EGL_CONTEXT: {
			egl_context_swap_buffers(context->egl);
		} break;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		case WGL_CONTEXT: {
			wgl_context_swap_buffers(context->wgl);
		} break;
#endif // WGL_ENABLED

		default:
			ls_log_fatal("Unknown OpenGL context type: %d\n", context->type);
	};

	// Check for new size after we swap the buffers
	os_mutex_lock(context->viewport_mutex);
	if (context->should_resize) {
		context->viewport_size = context->future_size;
		glViewport(0, 0, context->viewport_size.x, context->viewport_size.y);
		context->should_resize = false;
	}
	os_mutex_unlock(context->viewport_mutex);
}

void opengl_context_resize(OpenGLContext *context, Vector2u size) {
	LS_ASSERT(context);

	os_mutex_lock(context->viewport_mutex);
	context->future_size = size;
	context->should_resize = true;
	os_mutex_unlock(context->viewport_mutex);
}

Vector2u opengl_context_get_size(const OpenGLContext *context) {
	LS_ASSERT(context);

	os_mutex_lock(context->viewport_mutex);
	Vector2u size = context->viewport_size;
	os_mutex_unlock(context->viewport_mutex);

	return size;
}

const LSWindow *opengl_context_get_window(const OpenGLContext *context) {
	LS_ASSERT(context);

	return context->window;
}
