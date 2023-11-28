#ifndef RENDERER_INTERFACE_H
#define RENDERER_INTERFACE_H

#include "core/core.h"

typedef struct {
	void (*set_clear_color)(float32 r, float32 g, float32 b, float32 a);
	void (*clear)();
} RendererInterface;

#endif // RENDERER_INTERFACE_H