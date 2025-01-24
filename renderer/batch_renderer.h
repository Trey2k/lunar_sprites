#ifndef BETCH_RENDERER_H
#define BETCH_RENDERER_H

#include "core/core.h"

#include "renderer.h"
#include "shader.h"
#include "texture.h"

typedef struct {
	Vector3 pos;
	Vector2 tex_coords;
	Color color;
	// Used for SDF rounding
	Vector2 element_size;
	float32 radius;
} BatchVertex;

void batch_renderer_init();
void batch_renderer_deinit();

// Ends the current frame
void batch_renderer_end_frame();

// Queues a draw call to the batch renderer.
// Draw order is always maintained. Draw calls are batched by groups of 16 textures and 1024 vertices and or indices.
// There is a limit of 64 batch groups, if this limit is reached, the renderer will flush all batches early.
LS_EXPORT void batch_renderer_draw(const Texture *texture, const BatchVertex *vertices, const uint32 *indices, size_t nverts, size_t nindices);

LS_EXPORT void batch_renderer_draw_rect(const Texture *texture, Color color, uint32 radius, Vector2 position, Vector2u size);
LS_EXPORT void batch_renderer_draw_rect_outline(const Texture *texture, Color color, Color outline_color, uint32 radius, Vector2 position, Vector2u size, uint32 thickness);

#endif // BETCH_RENDERER_H