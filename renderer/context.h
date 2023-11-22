#ifndef CONTEXT_H
#define CONTEXT_H

#include "renderer/renderer.h"
#include "renderer/window.h"

typedef struct Context Context;

Context *renderer_context_create(const Renderer *renderer, const LSWindow *window);
void renderer_context_destroy(Context *context);

void renderer_context_make_current(const Context *context);
void renderer_context_swap_buffers(const Context *context);

#endif // CONTEXT_H