#ifndef CONTEXT_H
#define CONTEXT_H

#include "renderer/renderer.h"
#include "renderer/window.h"

Context *renderer_context_create(const Renderer *renderer, LSWindow *window);
void renderer_context_destroy(Context *context);

void renderer_context_make_current(Renderer *renderer, const Context *context);
void renderer_context_detach(Renderer *renderer, const Context *context);
void renderer_context_swap_buffers(const Context *context);

void renderer_context_resize(const Context *context, Vector2u size);

Vector2u renderer_context_get_size(const Context *context);

#endif // CONTEXT_H