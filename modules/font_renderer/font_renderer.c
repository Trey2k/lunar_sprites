#include "font_renderer.h"

#include "core/debug.h"
#include "core/events/events.h"
#include "core/memory.h"
#include "core/types/slice.h"

#include "renderer/buffers.h"
#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_array.h"
#include "renderer/window.h"

#define RFONT_IMPLEMENTATION
#define RFONT_NO_OPENGL

#define RFont_texture Texture *

#include <RFont.h>

struct Font {
	RFont_font *font;
};

Font *font_create(String font_path) {
	Font *font = ls_malloc(sizeof(Font));
	font->font = RFont_font_init(font_path);

	return font;
}

void font_destroy(Font *font) {
	RFont_font_free(font->font);
	ls_free(font);
}

typedef struct {
	const Renderer *renderer;
	Shader *shader;

	uint32 current_atlas;
	Color color;
	bool ready;

	VertexBuffer *vbo;
	float32 *vbo_data;
	size_t vbo_size;

	VertexArray *vao;
} FontRenderer;

extern const char *const FONT_SHADER_SOURCE;

static FontRenderer *font_renderer = NULL;

#define VBO_LAYOUT_COUNT 3
static const BufferElement VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
};

static void font_renderer_start(const LSWindow *window) {
	font_renderer->vbo = renderer_create_vertex_buffer_empty(font_renderer->renderer, BUFFER_USAGE_DYNAMIC);

	BufferLayout *vbo_layout = buffer_layout_create(VBO_LAYOUT, VBO_LAYOUT_COUNT);

	vertex_buffer_set_layout(font_renderer->vbo, vbo_layout);

	font_renderer->vao = renderer_create_vertex_array(font_renderer->renderer);

	font_renderer->shader = renderer_create_shader(font_renderer->renderer, FONT_SHADER_SOURCE, ls_str_length(FONT_SHADER_SOURCE));

	Vector2i size = window_get_size(window);
	RFont_init(size.x, size.y);
	font_renderer->ready = true;
}

static void event_handler(Event *event, void *user_data) {
	if (event->type != EVENT_WINDOW) {
		return;
	}

	EventWindow window_event = event->window;

	if (window_event.type == EVENT_WINDOW_OPEN) {
		if (!font_renderer->ready) {
			font_renderer_start(window_event.window);
		} else {
			Vector2i size = window_get_size(window_event.window);
			RFont_update_framebuffer(size.x, size.y);
		}
	} else if (window_event.type == EVENT_WINDOW_RESIZE && font_renderer->ready) {
		RFont_update_framebuffer(window_event.size.x, window_event.size.y);
	}
}

void font_renderer_init(Renderer *a_renderer, LSCore *core) {
	font_renderer = ls_malloc(sizeof(FontRenderer));
	font_renderer->renderer = a_renderer;
	font_renderer->current_atlas = 0;
	font_renderer->color = COLOR_WHITE;
	font_renderer->vbo_data = NULL;

	font_renderer->ready = false;
	EventManager *event_manager = core_get_event_manager(core);
	event_manager_add_handler(event_manager, event_handler, NULL);
}

void font_renderer_deinit() {
	vertex_array_destroy(font_renderer->vao);
	shader_destroy(font_renderer->shader);

	ls_free(font_renderer);
	RFont_close();
}

void font_renderer_set_color(Color color) {
	LS_ASSERT(font_renderer);
	font_renderer->color = color;
}

void font_renderer_draw_text(const Font *font, String text, float32 x, float32 y, uint32 size) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(font_renderer->renderer);

	if (!font_renderer->ready) {
		return;
	}

	RFont_draw_text(font->font, text, x, y, size);
}

Texture *font_renderer_create_atlas(uint32 atlas_width, uint32 atlas_height) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(font_renderer->renderer);

	font_renderer->current_atlas++;
	uint8 *atlas_data = ls_calloc(atlas_width * atlas_height, sizeof(uint8));
	Texture *atlas = texture_create(atlas_width, atlas_height, TEXTURE_FORMAT_R, atlas_data);
	ls_free(atlas_data);
	LS_ASSERT(atlas);

	return atlas;
}

void font_renderer_bitmap_to_atlas(Texture *atlas, uint8 *bitmap, float32 width, float32 height, float32 x, float32 y) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(font_renderer->renderer);
	LS_ASSERT(atlas);
	texture_add_sub_texture(atlas, TEXTURE_FORMAT_R, bitmap, x, y, width, height);
}

void font_renderer_render_text(Texture *atlas, float32 *vertices, float32 *tcoords, size_t nverts) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(font_renderer->renderer);

	if (!font_renderer->vbo_data) {
		font_renderer->vbo_size = nverts * (3 + 2 + 4) * sizeof(float32);
		font_renderer->vbo_data = ls_malloc(font_renderer->vbo_size);
	} else if (font_renderer->vbo_size < nverts * (3 + 2 + 4) * sizeof(float32)) {
		font_renderer->vbo_size = nverts * (3 + 2 + 4) * sizeof(float32);
		font_renderer->vbo_data = ls_realloc(font_renderer->vbo_data, font_renderer->vbo_size);
	}

	for (size_t i = 0; i < nverts; i++) {
		font_renderer->vbo_data[i * 9] = vertices[i * 3];
		font_renderer->vbo_data[i * 9 + 1] = vertices[i * 3 + 1];
		font_renderer->vbo_data[i * 9 + 2] = vertices[i * 3 + 2];

		font_renderer->vbo_data[i * 9 + 3] = tcoords[i * 2];
		font_renderer->vbo_data[i * 9 + 4] = tcoords[i * 2 + 1];

		font_renderer->vbo_data[i * 9 + 5] = font_renderer->color.r;
		font_renderer->vbo_data[i * 9 + 6] = font_renderer->color.g;
		font_renderer->vbo_data[i * 9 + 7] = font_renderer->color.b;
		font_renderer->vbo_data[i * 9 + 8] = font_renderer->color.a;
	}

	vertex_array_bind(font_renderer->vao);
	shader_bind(font_renderer->shader);

	vertex_buffer_set_data(font_renderer->vbo, font_renderer->vbo_data, font_renderer->vbo_size);
	const VertexBuffer *buffers[1] = { font_renderer->vbo };
	vertex_arrray_set_vertex_buffers(font_renderer->vao, buffers, 1);

	texture_bind(atlas, 0);

	vertex_array_draw(font_renderer->vao);
}

void font_renderer_free_atlas(Texture *atlas) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(font_renderer->renderer);

	texture_destroy(atlas);
}

// RFont overrides

void RFont_render_set_color(float r, float g, float b, float a) {
	font_renderer_set_color(color_create(r, g, b, a));
}

Texture *RFont_create_atlas(uint32 width, uint32 height) {
	return font_renderer_create_atlas(width, height);
}

void RFont_bitmap_to_atlas(Texture *atlas, uint8 *bitmap, float32 x, float32 y, float32 width, float32 height) {
	font_renderer_bitmap_to_atlas(atlas, bitmap, width, height, x, y);
}

void RFont_render_text(Texture *atlas, float32 *verts, float32 *tcoords, size_t nverts) {
	font_renderer_render_text(atlas, verts, tcoords, nverts);
}

void RFont_render_free(Texture *atlas) {
	font_renderer_free_atlas(atlas);
}

void RFont_render_init() {
}