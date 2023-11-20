#include "renderer/opengl/context.h"
#include "core/core.h"

#if defined(EGL_ENABLED)
#include "renderer/opengl/egl/context.h"
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
#include "renderer/opengl/wgl/context.h"
#endif // WGL_ENABLED

typedef enum {
	INVALID_CONTEXT,
	EGL_CONTEXT,
	WGL_CONTEXT,
} ContextType;

struct OpenGLContext {
	ContextType type;
	union {
#if defined(EGL_ENABLED)
		LSEGLContext *egl;
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
		LSWGLContext *wgl;
#endif // WGL_ENABLED
	};
};

OpenGLContext *opengl_context_create(const OpenGLRenderer *renderer, const LSWindow *window) {
	OpenGLContext *context = ls_malloc(sizeof(OpenGLContext));

#if defined(EGL_ENABLED)
	if (opengl_egl_enabled(renderer)) {
		context->egl = egl_context_create(window);
		if (context->egl) {
			context->type = EGL_CONTEXT;
			return context;
		}
	}
#endif // EGL_ENABLED

#if defined(WGL_ENABLED)
	if (opengl_wgl_enabled(renderer)) {
		context->wgl = wgl_context_create(window);
		if (context->wgl) {
			context->type = WGL_CONTEXT;
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

void opengl_context_swap_buffers(const OpenGLContext *context) {
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
}
