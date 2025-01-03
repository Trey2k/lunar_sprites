#include "batch_renderer.h"
#include "buffers.h"
#include "renderer/shader.h"
#include "shader.h"
#include "vertex_array.h"

#include "core/types/slice.h"

#define BATCH_VBO_LAYOUT_COUNT 3
static const BufferElement BATCH_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
};
#define BATCH_VBO_ROW_SIZE (3 + 2 + 4)
#define BATCH_VBO_ELEMENT_SIZE BATCH_VBO_ROW_SIZE * sizeof(float32)
#define VBO_MAX_SIZE 1024 * BATCH_VBO_ELEMENT_SIZE
#define IBO_MAX_SIZE 512 * sizeof(uint32)

extern const char *const DEFAULT_BATCH_SHADER_SOURCE;

typedef struct {
	const Texture *texture;
	const Shader *shader;
	float32 vbo_data[VBO_MAX_SIZE];
	uint32 ibo_data[IBO_MAX_SIZE];
	size_t nverts;
	size_t nindices;
	bool use_indices;
} Batch;

typedef struct {
	const Renderer *renderer;

	Shader *shader;

	VertexArray *vao;
	VertexBuffer *vbo;
	IndexBuffer *ibo;

	VertexArray *basic_vao;
	VertexBuffer *basic_vbo;

	Slice *batches;
	uint32 nbatches;
} BatchRenderer;

static BatchRenderer batch_renderer;

void batch_renderer_init(const Renderer *renderer) {
	batch_renderer.renderer = renderer;
	batch_renderer.shader = renderer_create_shader(renderer, DEFAULT_BATCH_SHADER_SOURCE, ls_str_length(DEFAULT_BATCH_SHADER_SOURCE));
	BufferLayout *vbo_layout = buffer_layout_create(BATCH_VBO_LAYOUT, BATCH_VBO_LAYOUT_COUNT);

	batch_renderer.vbo = renderer_create_vertex_buffer(renderer, NULL, VBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);
	vertex_buffer_set_layout(batch_renderer.vbo, vbo_layout);

	batch_renderer.ibo = renderer_create_index_buffer(renderer, NULL, IBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);

	batch_renderer.vao = renderer_create_vertex_array(renderer);

	batch_renderer.basic_vbo = renderer_create_vertex_buffer(renderer, NULL, VBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);
	vertex_buffer_set_layout(batch_renderer.basic_vbo, vbo_layout);

	batch_renderer.basic_vao = renderer_create_vertex_array(renderer);

	batch_renderer.batches = slice_create(16, true);
}

void batch_renderer_deinit() {
	vertex_buffer_destroy(batch_renderer.vbo);
	index_buffer_destroy(batch_renderer.ibo);
	vertex_array_destroy(batch_renderer.vao);

	vertex_buffer_destroy(batch_renderer.basic_vbo);
	vertex_array_destroy(batch_renderer.basic_vao);

	slice_clear(batch_renderer.batches);

	shader_destroy(batch_renderer.shader);
}

void batch_renderer_begin_frame() {
	batch_renderer.nbatches = 0;
}

static void batch_render(Batch *batch) {
	texture_bind(batch->texture, 0);
	shader_bind(batch->shader);

	vertex_array_bind(batch_renderer.vao);
	// Set sub data avoids reallocating the buffer
	vertex_buffer_set_sub_data(batch_renderer.vbo, batch->vbo_data, batch->nverts * BATCH_VBO_ELEMENT_SIZE, 0);

	const VertexBuffer *buffers[1] = { batch_renderer.vbo };
	vertex_array_set_vertex_buffers(batch_renderer.vao, buffers, 1);

	index_buffer_set_sub_data(batch_renderer.ibo, batch->ibo_data, batch->nindices * sizeof(uint32), 0);
	vertex_array_set_index_buffer(batch_renderer.vao, batch_renderer.ibo);

	vertex_array_draw_elements(batch_renderer.vao);

	batch->nverts = 0;
	batch->nindices = 0;

	shader_unbind(batch->shader);
	texture_unbind(batch->texture);
}

static void batch_render_no_indices(Batch *batch) {
	texture_bind(batch->texture, 0);
	shader_bind(batch->shader);

	vertex_array_bind(batch_renderer.basic_vao);
	// Set sub data avoids reallocating the buffer
	vertex_buffer_set_sub_data(batch_renderer.basic_vbo, batch->vbo_data, batch->nverts * BATCH_VBO_ELEMENT_SIZE, 0);

	const VertexBuffer *buffers[1] = { batch_renderer.basic_vbo };
	vertex_array_set_vertex_buffers(batch_renderer.basic_vao, buffers, 1);

	vertex_array_draw(batch_renderer.basic_vao);

	batch->nverts = 0;

	shader_unbind(batch->shader);
	texture_unbind(batch->texture);
}

void batch_renderer_end_frame() {
	for (size_t i = 0; i < batch_renderer.nbatches; i++) {
		Batch *batch = (Batch *)slice_get(batch_renderer.batches, i).ptr;

		if (batch->use_indices) {
			batch_render(batch);
		} else {
			batch_render_no_indices(batch);
		}
	}
}

void batch_renderer_draw(const Shader *shader, const Texture *texture, const float32 *vertices, const float32 *tex_coords, const uint32 *indices, Color color, size_t nverts, size_t nindices) {
	LS_ASSERT(nverts * BATCH_VBO_ELEMENT_SIZE < VBO_MAX_SIZE);
	const Shader *use_shader = shader ? shader : batch_renderer.shader;

	Batch *batch = NULL;

	bool use_indices = nindices > 0;
	for (size_t i = 0; i < slice_get_size(batch_renderer.batches); i++) {
		Batch *f_batch = (Batch *)slice_get(batch_renderer.batches, i).ptr;
		if (f_batch->texture == texture && f_batch->shader == use_shader && f_batch->use_indices == use_indices) {
			if (f_batch->nverts == 0) {
				batch_renderer.nbatches++;
			} else if ((f_batch->nverts + nverts) * BATCH_VBO_ELEMENT_SIZE > VBO_MAX_SIZE) {
				continue;
			}
			batch = f_batch;
			break;
		}
	}

	if (!batch) {
		batch = ls_malloc(sizeof(Batch));
		batch->texture = texture;
		batch->shader = use_shader;
		batch->nverts = 0;
		batch->nindices = 0;
		batch->use_indices = use_indices;

		slice_append(batch_renderer.batches, SLICE_VAL(ptr, batch));
		batch_renderer.nbatches++;
	}

	for (size_t i = 0; i < nverts; i++) {
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 0] = vertices[i * 3 + 0];
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 1] = vertices[i * 3 + 1];
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 2] = vertices[i * 3 + 2];

		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 3] = tex_coords[i * 2 + 0];
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 4] = tex_coords[i * 2 + 1];

		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 5] = color.r;
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 6] = color.g;
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 7] = color.b;
		batch->vbo_data[((batch->nverts + i) * BATCH_VBO_ROW_SIZE) + 8] = color.a;
	}
	if (!use_indices) {
		batch->nverts += nverts;
		return;
	}

	for (size_t i = 0; i < nindices; i++) {
		batch->ibo_data[batch->nindices + i] = indices[i] + batch->nverts;
	}
	batch->nverts += nverts;
	batch->nindices += nindices;
}