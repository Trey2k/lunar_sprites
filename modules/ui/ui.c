#include "ui.h"

#include "core/debug.h"
#include "core/types/slice.h"
#include "elements.h"

#include "renderer/buffers.h"
#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "renderer/vertex_array.h"
#include "renderer/window.h"

#include "main/lunar_sprites.h"

#define UI_BATCH_VBO_LAYOUT_COUNT 6
static const BufferElement UI_BATCH_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT2, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT, "tex_id", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
	{ SHADER_DATA_TYPE_FLOAT, "radius", false },
	{ SHADER_DATA_TYPE_FLOAT2, "element_size", false },
};

#define UI_BATCH_VBO_ROW_COUNT (2 + 2 + 1 + 4 + 1 + 2)
#define UI_BATCH_VBO_ROW_SIZE (UI_BATCH_VBO_ROW_COUNT * sizeof(float32))

#define UI_BATCH_VBO_MAX_SIZE 1024 * UI_BATCH_VBO_ROW_SIZE
#define UI_BATCH_IBO_MAX_SIZE 1024 * sizeof(uint32)

extern const char *const UI_SHADER_SOURCE;

typedef struct {
	const Renderer *renderer;
	const LSWindow *window;

	const Texture *texture[16];
	size_t n_textures;

	Shader *shader;

	VertexBuffer *vbo;
	IndexBuffer *ibo;
	VertexArray *vao;

	float32 vbo_data[UI_BATCH_VBO_MAX_SIZE];
	uint32 ibo_data[UI_BATCH_IBO_MAX_SIZE];

	uint32 vbo_size;
	uint32 ibo_size;
	uint32 vert_count;

	Slice *elements;
} UIRenderer;

static UIRenderer ui_renderer;

static void ui_flush_batch();

static void ui_on_update(float64 delta_time) {
	size_t n_elements = slice_get_size(ui_renderer.elements);
	Vector2u inner_bounds = vec2u(0, 0);
	Vector2u outer_bounds = window_get_size(ui_renderer.window);
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *element = slice_get(ui_renderer.elements, i).ptr;
		// Root elements bounds are the window size.

		// Updated element size and position
		ui_element_calculate_position(element, outer_bounds, inner_bounds);
		ui_draw_element(element);
	}

	ui_flush_batch();
}

void ui_init(Renderer *renderer, LSCore *core, const LSWindow *window) {
	ui_renderer.shader = renderer_create_shader(renderer, UI_SHADER_SOURCE, ls_str_length(UI_SHADER_SOURCE));

	BufferLayout *vbo_layout = buffer_layout_create(UI_BATCH_VBO_LAYOUT, UI_BATCH_VBO_LAYOUT_COUNT);
	ui_renderer.vbo = renderer_create_vertex_buffer(renderer, NULL, UI_BATCH_VBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);
	vertex_buffer_set_layout(ui_renderer.vbo, vbo_layout);

	ui_renderer.ibo = renderer_create_index_buffer(renderer, NULL, UI_BATCH_IBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);

	ui_renderer.vao = renderer_create_vertex_array(renderer);

	ui_renderer.vbo_size = 0;
	ui_renderer.ibo_size = 0;
	ui_renderer.vert_count = 0;

	for (size_t i = 0; i < 16; i++) {
		ui_renderer.texture[i] = NULL;
	}

	ui_renderer.renderer = renderer;
	ui_renderer.window = window;
	ui_renderer.elements = slice_create(16, false);

	// Called before the main frame for now, might change later.
	ls_register_update_callback(ui_on_update, true);
}

void ui_deinit() {
	shader_destroy(ui_renderer.shader);

	vertex_buffer_destroy(ui_renderer.vbo);
	index_buffer_destroy(ui_renderer.ibo);
	vertex_array_destroy(ui_renderer.vao);

	for (size_t i = 0; i < slice_get_size(ui_renderer.elements); i++) {
		UIElement *element = slice_get(ui_renderer.elements, i).ptr;
		ui_element_destroy(element);
	}

	slice_destroy(ui_renderer.elements);
}

void ui_flush_batch() {
	if (ui_renderer.vbo_size == 0) {
		return;
	}

	vertex_array_bind(ui_renderer.vao);
	vertex_buffer_set_sub_data(ui_renderer.vbo, ui_renderer.vbo_data, ui_renderer.vbo_size * sizeof(float32), 0);
	index_buffer_set_sub_data(ui_renderer.ibo, ui_renderer.ibo_data, ui_renderer.ibo_size * sizeof(uint32), 0);

	const VertexBuffer *buffers[1] = { ui_renderer.vbo };
	vertex_array_set_vertex_buffers(ui_renderer.vao, buffers, 1);
	vertex_array_set_index_buffer(ui_renderer.vao, ui_renderer.ibo);

	shader_bind(ui_renderer.shader);

	for (size_t i = 0; i < ui_renderer.n_textures; i++) {
		texture_bind(ui_renderer.texture[i], i);
	}

	Vector2u viewport_size = window_get_size(ui_renderer.window);
	shader_set_uniform_vec2(ui_renderer.shader, "u_resolution", vec2((float32)viewport_size.x, (float32)viewport_size.y));
	vertex_array_draw_elements(ui_renderer.vao);

	shader_unbind(ui_renderer.shader);

	ui_renderer.vbo_size = 0;
	ui_renderer.ibo_size = 0;
	ui_renderer.vert_count = 0;
	ui_renderer.n_textures = 0;
}

float32 ui_add_texture(const Texture *texture) {
	if (!texture) {
		return -1.0f;
	}

	for (size_t i = 0; i < ui_renderer.n_textures; i++) {
		if (ui_renderer.texture[i] == texture) {
			return (float32)i;
		}
	}

	if (ui_renderer.n_textures >= 16) {
		ui_flush_batch();
	}

	ui_renderer.texture[ui_renderer.n_textures] = texture;
	return (float32)ui_renderer.n_textures++;
}

void ui_add_element(UIElement *element) {
	slice_append(ui_renderer.elements, SLICE_VAL(ptr, (void *)element));
}

void ui_remove_element(const UIElement *element) {
	size_t n_elements = slice_get_size(ui_renderer.elements);
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *elm = slice_get(ui_renderer.elements, i).ptr;
		if (elm == element) {
			slice_remove(ui_renderer.elements, i);
			ui_element_destroy(elm);
			break;
		}
	}
}

static void ui_font_render_callback(const Texture *atlas, float32 *verts, float32 *tcoords, size_t nverts, void *user_data) {
	LS_ASSERT(atlas);

	UIElementTheme *theme = (UIElementTheme *)user_data;
	LS_ASSERT(theme);

	if ((ui_renderer.vert_count * UI_BATCH_VBO_ROW_SIZE) + (nverts * UI_BATCH_VBO_ROW_SIZE) > UI_BATCH_VBO_MAX_SIZE) {
		ui_flush_batch();
	}

	float32 tex_id = ui_add_texture(atlas);

	for (size_t i = 0; i < nverts; i++) {
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = verts[i * 3];
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = verts[i * 3 + 1];

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tcoords[i * 2];
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tcoords[i * 2 + 1];

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tex_id;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = theme->font_color.r;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = theme->font_color.g;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = theme->font_color.b;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = theme->font_color.a;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = 0.0f;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = 0.0f;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = 0.0f;
	}

	for (size_t i = 0; i < nverts; i++) {
		ui_renderer.ibo_data[ui_renderer.ibo_size++] = i + ui_renderer.vert_count;
	}

	ui_renderer.vert_count += nverts;
}

void ui_draw(const Texture *texture, Color color, const float32 *vertices, const float32 *tcoords, const uint32 *indices, size_t nverts, size_t nindices, uint32 radius, Vector2u element_size) {
	LS_ASSERT(nverts * UI_BATCH_VBO_ROW_SIZE <= UI_BATCH_VBO_MAX_SIZE);
	LS_ASSERT(ui_renderer.shader);
	LS_ASSERT(ui_renderer.vbo);
	LS_ASSERT(ui_renderer.ibo);
	LS_ASSERT(ui_renderer.vao);

	if ((ui_renderer.vert_count * UI_BATCH_VBO_ROW_SIZE) + (nverts * UI_BATCH_VBO_ROW_SIZE) > UI_BATCH_VBO_MAX_SIZE ||
			(ui_renderer.ibo_size * sizeof(uint32)) + (nindices * sizeof(uint32)) > UI_BATCH_IBO_MAX_SIZE) {
		ui_flush_batch();
	}

	float32 tex_id = ui_add_texture(texture);

	for (size_t i = 0; i < nverts; i++) {
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = vertices[i * 2];
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = vertices[i * 2 + 1];

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tcoords[i * 2];
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tcoords[i * 2 + 1];

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = tex_id;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = color.r;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = color.g;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = color.b;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = color.a;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = (float32)radius;

		ui_renderer.vbo_data[ui_renderer.vbo_size++] = (float32)element_size.x;
		ui_renderer.vbo_data[ui_renderer.vbo_size++] = (float32)element_size.y;
	}
	for (size_t i = 0; i < nindices; i++) {
		ui_renderer.ibo_data[ui_renderer.ibo_size++] = indices[i] + ui_renderer.vert_count;
	}

	ui_renderer.vert_count += nverts;
}

Vector2u ui_draw_text(const UIElementTheme *theme, String text, Vector2u position) {
	LS_ASSERT(theme);
	LS_ASSERT(theme->font);

	return font_draw_text_callback(theme->font, theme->font_size, text, (float32)position.x, (float32)position.y, ui_font_render_callback, (void *)theme);
}

Vector2u ui_get_text_size(const UIElementTheme *theme, String text) {
	LS_ASSERT(theme);
	LS_ASSERT(theme->font);

	return font_get_text_size(theme->font, theme->font_size, text);
}

// Draws a rectangle with the given theme, positioned from the top-left corner.
// Size is given in pixels.
void ui_draw_rect(const Texture *texture, Color color, uint32 radius, Vector2u position, Vector2u size) {
	Vector2u viewport_size = window_get_size(ui_renderer.window);
	float32 x = (float32)position.x / (float32)viewport_size.x * 2.0f - 1.0f;
	float32 y = 1.0f - (float32)position.y / (float32)viewport_size.y * 2.0f;
	float32 w = (float32)size.x / (float32)viewport_size.x * 2.0f;
	float32 h = (float32)size.y / (float32)viewport_size.y * 2.0f;

	float32 vertices[] = {
		x,
		y,
		x + w,
		y,
		x + w,
		y - h,
		x,
		y - h,
	};

	float32 tcoords[] = {
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
	};

	uint32 indices[] = {
		0,
		1,
		2,
		2,
		3,
		0,
	};

	ui_draw(texture, color, vertices, tcoords, indices, 4, 6, radius, size);
}
