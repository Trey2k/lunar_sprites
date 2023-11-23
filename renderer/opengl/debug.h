#ifndef OPENGL_CALL_H
#define OPENGL_CALL_H

#include "core/core.h"
#include <glad/gl.h>

_FORCE_INLINE_ String opengl_err_string(GLenum error) {
	switch (error) {
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case 0x0504:
			return "GL_STACK_UNDERFLOW";
		case 0x0503:
			return "GL_STACK_OVERFLOW";
		default:
			return "UNKNOWN";
	}
}

#if defined(DEBUG_ENABLED)
#define GL_CALL(func)                                                                                      \
	func;                                                                                                  \
	{                                                                                                      \
		GLenum error = glGetError();                                                                       \
		if (error != GL_NO_ERROR) {                                                                        \
			ls_log(LOG_LEVEL_ERROR, "OpenGL error: %s\n\tGL_CALL(%s)\n\tFunction %s, file %s, line %d.\n", \
					(String)opengl_err_string(error), _MKSTR(func), __FUNCTION__, __FILE__, __LINE__);     \
			LS_BREAKPOINT();                                                                               \
		}                                                                                                  \
	}
#else // DEBUG_ENABLED
#define GL_CALL(func) func;
#endif // DEBUG_ENABLED

#endif // OPENGL_CALL_H