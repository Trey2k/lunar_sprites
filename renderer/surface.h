#ifndef SURFACE_H
#define SURFACE_H

#include "renderer/renderer.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/opengl3_renderer.h"
#endif

typedef struct {
	union {
#if defined(OPENGL3_ENABLED)
		OpenGL3Surface opengl3_surface;
#endif
	};
} Surface;

#endif // SURFACE_H
