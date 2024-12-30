#ifndef BETCH_RENDERER_H
#define BETCH_RENDERER_H

#include "core/core.h"

#include "renderer.h"
#include "shader.h"
#include "texture.h"

void batch_renderer_init(const Renderer *renderer);
void batch_renderer_deinit();

// Begins a new frame
void batch_renderer_begin_frame();
// Ends the current frame
void batch_renderer_end_frame();

// Queues a basic draw operation if shader is NULL, the default shader will be used
LS_EXPORT void batch_renderer_draw(const Shader *shader, const Texture *texture, const float32 *vertices, const float32 *tex_coords, const uint32 *indices, Color color, size_t nverts, size_t nindices);

#endif // BETCH_RENDERER_H