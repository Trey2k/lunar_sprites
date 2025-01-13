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
	const LSWindow *window;

	bool should_resize;
	Vector2u size;

	LSMutex *mutex;

	union {
#if defined(EGL_ENABLED)
		LSEGLContext *egl;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		LSWGLContext *wgl;
#endif // WGL_ENABLED
	};
};

static void opengl_context_event_handler(Event *event, void *user_data) {
	OpenGLContext *context = user_data;

	os_mutex_lock(context->mutex);

	switch (event->type) {
		case EVENT_WINDOW: {
			if (event->window.type != EVENT_WINDOW_RESIZE) {
				break;
			}

			if (event->window.window != context->window) {
				break;
			}

			// Events are processed in the main thread, so we can safely update the context here.
			context->size = event->window.size;
			context->should_resize = true;
		} break;

		default:
			break;
	};

	os_mutex_unlock(context->mutex);
}

static void opengl_init(OpenGLContext *context, const LSCore *core, const LSWindow *window) {
	os_mutex_lock(context->mutex);
	GL_CALL(glEnable(GL_BLEND));
	GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	Vector2u size = window_get_size(window);
	GL_CALL(glViewport(0, 0, size.x, size.y));
	EventManager *event_manager = core_get_event_manager(core);
	event_manager_add_handler(event_manager, opengl_context_event_handler, context);
	os_mutex_unlock(context->mutex);
}

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, const LSWindow *window) {
	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));
	context->window = window;
	context->should_resize = false;
	context->mutex = os_mutex_create();

#if defined(EGL_ENABLED)
	if (opengl_egl_enabled(renderer)) {
		context->egl = egl_context_create(window);
		if (context->egl) {
			context->type = EGL_CONTEXT;
			opengl_context_make_current(context);
			opengl_context_swap_buffers(context);
			opengl_init(context, opengl_renderer_get_core(renderer), window);
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
			opengl_init(context, opengl_renderer_get_core(renderer), window);
			return context;
		}
	}
#endif // WGL_ENABLED

	ls_free(context);
	return NULL;
}

void opengl_context_destroy(OpenGLContext *context) {
	LS_ASSERT(context);

	os_mutex_destroy(context->mutex);

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

	os_mutex_lock(context->mutex);

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

	os_mutex_unlock(context->mutex);
}

void opengl_context_detach(const OpenGLContext *context) {
	LS_ASSERT(context);

	os_mutex_lock(context->mutex);

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

	os_mutex_unlock(context->mutex);
}

void opengl_context_swap_buffers(OpenGLContext *context) {
	LS_ASSERT(context);
	os_mutex_lock(context->mutex);

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

	if (context->should_resize) {
		Vector2u size = context->size;
		GL_CALL(glViewport(0, 0, size.x, size.y));
		context->should_resize = false;
	}

	os_mutex_unlock(context->mutex);
}

const LSWindow *opengl_context_get_window(const OpenGLContext *context) {
	LS_ASSERT(context);

	return context->window;
}