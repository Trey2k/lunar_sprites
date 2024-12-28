#include "batch_renderer.h"
#include "buffers.h"
#include "renderer/shader.h"
#include "shader.h"
#include "vertex_array.h"

#include "core/types/slice.h"

#define BATCH_VBO_LAYOUT_COUNT 4
static const BufferElement BATCH_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT, "tex_id", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
};
#define BATCH_VBO_ROW_SIZE (3 + 1 + 2 + 4)
#define BATCH_VBO_ELEMENT_SIZE BATCH_VBO_ROW_SIZE * sizeof(float32)

extern const char *const BASIC_BATCH_SHADER_SOURCE;

typedef struct {
	const Renderer *renderer;

	Shader *shader;

	VertexArray *basic_vao;
	VertexBuffer *basic_vbo;
	size_t basic_vbo_size;
	Slice32 *basic_vbo_data;
	size_t basic_nverts;
	const Texture *basic_textures[16];
	size_t basic_texture_count;

	VertexArray *vao;
	VertexBuffer *vbo;
	IndexBuffer *ibo;
	size_t vbo_size;
	Slice32 *vbo_data;
	size_t ibo_size;
	Slice32 *ibo_data;
	size_t nverts;
	size_t nindices;
	const Texture *textures[16];
	size_t texture_count;
} BatchRenderer;

static BatchRenderer batch_renderer;

void batch_renderer_init(const Renderer *renderer) {
	batch_renderer.renderer = renderer;
	batch_renderer.shader = renderer_create_shader(renderer, BASIC_BATCH_SHADER_SOURCE, ls_str_length(BASIC_BATCH_SHADER_SOURCE));
	BufferLayout *vbo_layout = buffer_layout_create(BATCH_VBO_LAYOUT, BATCH_VBO_LAYOUT_COUNT);

	batch_renderer.basic_texture_count = 0;
	batch_renderer.basic_vbo_data = slice32_create(1024 * BATCH_VBO_ROW_SIZE);
	batch_renderer.basic_vbo = renderer_create_vertex_buffer(renderer, NULL, 1024 * BATCH_VBO_ELEMENT_SIZE, BUFFER_USAGE_DYNAMIC);
	batch_renderer.basic_vbo_size = 1024 * BATCH_VBO_ELEMENT_SIZE;
	vertex_buffer_set_layout(batch_renderer.basic_vbo, vbo_layout);
	batch_renderer.basic_vao = renderer_create_vertex_array(renderer);

	batch_renderer.texture_count = 0;
	batch_renderer.vbo_data = slice32_create(1024 * BATCH_VBO_ROW_SIZE);
	batch_renderer.vbo = renderer_create_vertex_buffer(renderer, NULL, 1024 * BATCH_VBO_ELEMENT_SIZE, BUFFER_USAGE_DYNAMIC);
	batch_renderer.vbo_size = 1024 * BATCH_VBO_ELEMENT_SIZE;
	vertex_buffer_set_layout(batch_renderer.vbo, vbo_layout);
	batch_renderer.ibo_data = slice32_create(1024);
	batch_renderer.ibo = renderer_create_index_buffer(renderer, NULL, 1024 * sizeof(uint32), BUFFER_USAGE_DYNAMIC);
	batch_renderer.ibo_size = 1024 * sizeof(uint32);
	batch_renderer.vao = renderer_create_vertex_array(renderer);
}

void batch_renderer_deinit() {
	slice32_destroy(batch_renderer.basic_vbo_data);
	vertex_buffer_destroy(batch_renderer.basic_vbo);
	vertex_array_destroy(batch_renderer.basic_vao);

	slice32_destroy(batch_renderer.vbo_data);
	vertex_buffer_destroy(batch_renderer.vbo);
	slice32_destroy(batch_renderer.ibo_data);
	index_buffer_destroy(batch_renderer.ibo);
	vertex_array_destroy(batch_renderer.vao);

	shader_destroy(batch_renderer.shader);
}

void batch_renderer_begin_frame() {
	slice32_clear(batch_renderer.basic_vbo_data);
	slice32_clear(batch_renderer.vbo_data);
	slice32_clear(batch_renderer.ibo_data);
	batch_renderer.basic_nverts = 0;
	batch_renderer.nverts = 0;
	batch_renderer.nindices = 0;
	batch_renderer.basic_texture_count = 0;
	batch_renderer.texture_count = 0;
}

void batch_renderer_end_frame() {
	vertex_array_bind(batch_renderer.basic_vao);
	if (batch_renderer.basic_nverts * BATCH_VBO_ELEMENT_SIZE > batch_renderer.basic_vbo_size) {
		vertex_buffer_set_data(batch_renderer.basic_vbo, slice32_get_data(batch_renderer.basic_vbo_data), batch_renderer.basic_nverts * BATCH_VBO_ELEMENT_SIZE);
	} else {
		// Set sub data avoids reallocating the buffer
		vertex_buffer_set_sub_data(batch_renderer.basic_vbo, slice32_get_data(batch_renderer.basic_vbo_data), batch_renderer.basic_nverts * BATCH_VBO_ELEMENT_SIZE, 0);
	}
	const VertexBuffer *buffers[1] = { batch_renderer.basic_vbo };
	vertex_array_set_vertex_buffers(batch_renderer.basic_vao, buffers, 1);

	vertex_array_bind(batch_renderer.vao);
	if (batch_renderer.nverts * BATCH_VBO_ELEMENT_SIZE > batch_renderer.vbo_size) {
		vertex_buffer_set_data(batch_renderer.vbo, slice32_get_data(batch_renderer.vbo_data), batch_renderer.nverts * BATCH_VBO_ELEMENT_SIZE);
	} else {
		// Set sub data avoids reallocating the buffer
		vertex_buffer_set_sub_data(batch_renderer.vbo, slice32_get_data(batch_renderer.vbo_data), batch_renderer.nverts * BATCH_VBO_ELEMENT_SIZE, 0);
	}

	if (batch_renderer.nindices * sizeof(uint32) > batch_renderer.ibo_size) {
		index_buffer_set_data(batch_renderer.ibo, slice32_get_data(batch_renderer.ibo_data), batch_renderer.nindices * sizeof(uint32));
	} else {
		index_buffer_set_sub_data(batch_renderer.ibo, slice32_get_data(batch_renderer.ibo_data), batch_renderer.nindices * sizeof(uint32), 0);
	}

	const VertexBuffer *buffers2[1] = { batch_renderer.vbo };
	vertex_array_set_vertex_buffers(batch_renderer.vao, buffers2, 1);
	vertex_array_set_index_buffer(batch_renderer.vao, batch_renderer.ibo);
}

void batch_renderer_flush() {
	for (size_t i = 0; i < batch_renderer.basic_texture_count; i++) {
		texture_bind(batch_renderer.basic_textures[i], i);
	}

	shader_bind(batch_renderer.shader);

	vertex_array_draw(batch_renderer.basic_vao);
	batch_renderer.basic_nverts = 0;

	slice32_clear(batch_renderer.basic_vbo_data);

	// Clear the textures
	for (size_t i = 0; i < batch_renderer.basic_texture_count; i++) {
		texture_unbind(batch_renderer.basic_textures[i]);
	}

	batch_renderer.basic_texture_count = 0;

	for (size_t i = 0; i < batch_renderer.texture_count; i++) {
		texture_bind(batch_renderer.textures[i], i);
	}

	shader_bind(batch_renderer.shader);

	vertex_array_draw_elements(batch_renderer.vao);
	batch_renderer.nverts = 0;
	batch_renderer.nindices = 0;

	slice32_clear(batch_renderer.vbo_data);
	slice32_clear(batch_renderer.ibo_data);

	// Clear the textures
	for (size_t i = 0; i < batch_renderer.texture_count; i++) {
		texture_unbind(batch_renderer.textures[i]);
	}
}

void batch_renderer_draw_basic(const Texture *texture, const float32 *vertices, const float32 *tex_coords, Color color, size_t nverts) {
	int32 tex_id = -1;
	for (size_t i = 0; i < batch_renderer.basic_texture_count; i++) {
		if (batch_renderer.basic_textures[i] == texture) {
			tex_id = i;
			break;
		}
	}

	if (tex_id == -1) {
		if (batch_renderer.basic_texture_count >= 16) {
			LS_ASSERT_MSG(false, "%s", "Batch renderer only supports 16 textures. Implement batching by texture you dummy.");
			return;
		}

		tex_id = batch_renderer.basic_texture_count;
		batch_renderer.basic_textures[batch_renderer.basic_texture_count] = texture;
		batch_renderer.basic_texture_count++;
	}

	for (size_t i = 0; i < nverts; i++) {
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, vertices[i * 3]));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, vertices[i * 3 + 1]));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, vertices[i * 3 + 2]));

		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, tex_id));

		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, tex_coords[i * 2]));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, tex_coords[i * 2 + 1]));

		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, color.r));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, color.g));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, color.b));
		slice32_append(batch_renderer.basic_vbo_data, SLICE_VAL32(f32, color.a));
	}

	batch_renderer.basic_nverts += nverts;
}

void batch_renderer_draw(const Texture *texture, const float32 *vertices, const float32 *tex_coords, const uint32 *indices, Color color, size_t nverts, size_t nindices) {
	int32 tex_id = -1;
	for (size_t i = 0; i < batch_renderer.texture_count; i++) {
		if (batch_renderer.textures[i] == texture) {
			tex_id = i;
			break;
		}
	}

	if (tex_id == -1) {
		if (batch_renderer.texture_count >= 16) {
			LS_ASSERT_MSG(false, "%s", "Batch renderer only supports 16 textures. Implement batching by texture you dummy.");
			return;
		}

		tex_id = batch_renderer.texture_count;
		batch_renderer.textures[batch_renderer.texture_count] = texture;
		batch_renderer.texture_count++;
	}

	for (size_t i = 0; i < nverts; i++) {
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, vertices[i * 3]));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, vertices[i * 3 + 1]));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, vertices[i * 3 + 2]));

		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, tex_id));

		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, tex_coords[i * 2]));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, tex_coords[i * 2 + 1]));

		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, color.r));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, color.g));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, color.b));
		slice32_append(batch_renderer.vbo_data, SLICE_VAL32(f32, color.a));
	}

	for (size_t i = 0; i < nindices; i++) {
		slice32_append(batch_renderer.ibo_data, SLICE_VAL32(u32, indices[i]));
	}

	batch_renderer.nverts += nverts;
	batch_renderer.nindices += nindices;
}