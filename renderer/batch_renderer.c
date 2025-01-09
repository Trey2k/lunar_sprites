#include "batch_renderer.h"
#include "buffers.h"
#include "core/memory.h"
#include "renderer/shader.h"
#include "shader.h"
#include "vertex_array.h"

#define BATCH_VBO_LAYOUT_COUNT 5
static const BufferElement BATCH_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
	{ SHADER_DATA_TYPE_FLOAT2, "element_size", false },
	{ SHADER_DATA_TYPE_FLOAT, "radius", false },
};

#define BATCH_TEXID_VBO_LAYOUT_COUNT 1
static const BufferElement BATCH_TEXID_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT, "tex_id", false },
};

static const int32 BATCH_TEXT_IDS[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

extern const char *const BATCH_SHADER_SOURCE;

#define VBO_MAX_ELEMENTS 1024
#define IBO_MAX_ELEMENTS 1024

typedef struct {
	const Texture *textures[16];

	BatchVertex vertices[VBO_MAX_ELEMENTS];
	float32 tex_ids[VBO_MAX_ELEMENTS];
	uint32 indices[IBO_MAX_ELEMENTS];

	size_t nverts;
	size_t nindices;
	uint8 ntextures;
} Batch;

typedef struct {
	const Renderer *renderer;

	Shader *shader;

	VertexArray *vao;
	VertexBuffer *vbo;
	VertexBuffer *texid_vbo;
	IndexBuffer *ibo;

	Batch batches[64];
	uint32 nbatches;
} BatchRenderer;

static BatchRenderer *batch_renderer;

static void batch_renderer_flush();

static void draw_batch(Batch *batch);

void batch_renderer_init(const Renderer *renderer) {
	batch_renderer = ls_malloc(sizeof(BatchRenderer));

	batch_renderer->renderer = renderer;

	batch_renderer->vao = renderer_create_vertex_array(renderer);

	batch_renderer->vbo = renderer_create_vertex_buffer(renderer, NULL, VBO_MAX_ELEMENTS * sizeof(BatchVertex), BUFFER_USAGE_DYNAMIC);
	BufferLayout *vbo_layout = buffer_layout_create(BATCH_VBO_LAYOUT, BATCH_VBO_LAYOUT_COUNT);
	vertex_buffer_set_layout(batch_renderer->vbo, vbo_layout);

	batch_renderer->texid_vbo = renderer_create_vertex_buffer(renderer, NULL, VBO_MAX_ELEMENTS * sizeof(float32), BUFFER_USAGE_DYNAMIC);
	BufferLayout *texid_vbo_layout = buffer_layout_create(BATCH_TEXID_VBO_LAYOUT, BATCH_TEXID_VBO_LAYOUT_COUNT);
	vertex_buffer_set_layout(batch_renderer->texid_vbo, texid_vbo_layout);

	batch_renderer->ibo = renderer_create_index_buffer(renderer, NULL, IBO_MAX_ELEMENTS * sizeof(uint32), BUFFER_USAGE_DYNAMIC);

	batch_renderer->shader = renderer_create_shader(renderer, BATCH_SHADER_SOURCE, ls_str_length(BATCH_SHADER_SOURCE));

	batch_renderer->nbatches = 0;
}

void batch_renderer_deinit() {
	vertex_array_destroy(batch_renderer->vao);
	vertex_buffer_destroy(batch_renderer->vbo);
	vertex_buffer_destroy(batch_renderer->texid_vbo);
	index_buffer_destroy(batch_renderer->ibo);

	shader_destroy(batch_renderer->shader);

	ls_free(batch_renderer);
}

void batch_renderer_begin_frame() {
	batch_renderer->nbatches = 0;
}

void batch_renderer_end_frame() {
	batch_renderer_flush();
}

void batch_renderer_draw(const Texture *texture, const BatchVertex *vertices, const uint32 *indices, size_t nverts, size_t nindices) {
	if (batch_renderer->nbatches == 0) {
		batch_renderer->batches[0].nverts = 0;
		batch_renderer->batches[0].nindices = 0;

		batch_renderer->nbatches++;
	}

	Batch *current_batch = &batch_renderer->batches[batch_renderer->nbatches - 1];

	if (current_batch->nverts + nverts > VBO_MAX_ELEMENTS || current_batch->nindices + nindices > IBO_MAX_ELEMENTS) {
		if (batch_renderer->nbatches == 64) {
			ls_log(LOG_LEVEL_WARNING, "Batch renderer reached maximum number of batches\n");
			batch_renderer_flush();
		}

		current_batch = &batch_renderer->batches[batch_renderer->nbatches];
		current_batch->nverts = 0;
		current_batch->nindices = 0;

		batch_renderer->nbatches++;
	}

	float32 tex_id = -1;
	if (texture) {
		for (uint8 i = 0; i < current_batch->ntextures; i++) {
			if (current_batch->textures[i] == texture) {
				tex_id = i;
				break;
			}
		}

		if (tex_id == -1) {
			if (current_batch->ntextures < 16) {
				tex_id = (float32)current_batch->ntextures;
				current_batch->textures[current_batch->ntextures++] = texture;
			} else {
				if (batch_renderer->nbatches == 64) {
					ls_log(LOG_LEVEL_WARNING, "Batch renderer reached maximum number of batches\n");
					batch_renderer_flush();
				}

				current_batch = &batch_renderer->batches[batch_renderer->nbatches];
				current_batch->nverts = 0;
				current_batch->nindices = 0;
				current_batch->ntextures = 1;
				current_batch->textures[0] = texture;
			}
		}
	}

	ls_memcpy(&current_batch->vertices[current_batch->nverts], vertices, nverts * sizeof(BatchVertex));

	for (size_t i = 0; i < nverts; i++) {
		current_batch->tex_ids[current_batch->nverts + i] = tex_id;
	}

	for (size_t i = 0; i < nindices; i++) {
		current_batch->indices[current_batch->nindices + i] = indices[i] + current_batch->nverts;
	}

	current_batch->nverts += nverts;
	current_batch->nindices += nindices;
}

static void batch_renderer_flush() {
	for (uint32 i = 0; i < batch_renderer->nbatches; i++) {
		draw_batch(&batch_renderer->batches[i]);
	}

	batch_renderer->nbatches = 0;
}

static void draw_batch(Batch *batch) {
	vertex_array_bind(batch_renderer->vao);

	vertex_buffer_set_sub_data(batch_renderer->vbo, batch->vertices, batch->nverts * sizeof(BatchVertex), 0);
	vertex_buffer_set_sub_data(batch_renderer->texid_vbo, batch->tex_ids, batch->nverts * sizeof(float32), 0);
	index_buffer_set_sub_data(batch_renderer->ibo, batch->indices, batch->nindices * sizeof(uint32), 0);

	const VertexBuffer *buffers[2] = { batch_renderer->vbo, batch_renderer->texid_vbo };
	vertex_array_set_vertex_buffers(batch_renderer->vao, buffers, 2);
	vertex_array_set_index_buffer(batch_renderer->vao, batch_renderer->ibo);

	for (uint8 i = 0; i < batch->ntextures; i++) {
		texture_bind(batch->textures[i], i);
	}

	shader_bind(batch_renderer->shader);
	shader_set_uniform_intv(batch_renderer->shader, "u_textures", BATCH_TEXT_IDS, batch->ntextures);

	Vector2u viewport_size = renderer_get_viewport_size(batch_renderer->renderer);
	shader_set_uniform_vec3(batch_renderer->shader, "u_resolution", vec3(viewport_size.x, viewport_size.y, 0.0));

	vertex_array_draw_elements(batch_renderer->vao);

	shader_unbind(batch_renderer->shader);
}

void batch_renderer_draw_rect(const Texture *texture, Color color, uint32 radius, Vector2 position, Vector2u size) {
	BatchVertex vertices[4];
	static const uint32 indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	Vector2u viewport_size = renderer_get_viewport_size(batch_renderer->renderer);

	float32 x = (float32)position.x / (float32)viewport_size.x * 2.0f - 1.0f;
	float32 y = 1.0f - (float32)position.y / (float32)viewport_size.y * 2.0f;

	float32 w = (float32)size.x / (float32)viewport_size.x * 2.0f;
	float32 h = (float32)size.y / (float32)viewport_size.y * 2.0f;

	vertices[0].pos = vec3(x, y, 0.0f);
	vertices[0].tex_coords = vec2(0.0f, 0.0f);
	vertices[1].pos = vec3(x + w, y, 0.0f);
	vertices[1].tex_coords = vec2(1.0f, 0.0f);
	vertices[2].pos = vec3(x + w, y - h, 0.0f);
	vertices[2].tex_coords = vec2(1.0f, 1.0f);
	vertices[3].pos = vec3(x, y - h, 0.0f);
	vertices[3].tex_coords = vec2(0.0f, 1.0f);

	for (size_t i = 0; i < 4; i++) {
		vertices[i].color = color;
		vertices[i].element_size = vec2(size.x, size.y);
		vertices[i].radius = (float32)radius;
	}

	batch_renderer_draw(texture, vertices, indices, 4, 6);
}

void batch_renderer_draw_rect_outline(const Texture *texture, Color color, Color outline_color, uint32 radius, Vector2 position, Vector2u size, uint32 thickness) {
	batch_renderer_draw_rect(texture, outline_color, radius, position, size);
	Vector2 inner_position = vec2(position.x + thickness, position.y + thickness);
	Vector2u inner_size = vec2u(size.x - thickness * 2, size.y - thickness * 2);
	batch_renderer_draw_rect(texture, color, radius, inner_position, inner_size);
}