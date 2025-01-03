#include "font.h"

#include "core/debug.h"
#include "core/events/events.h"
#include "core/memory.h"
#include "core/types/color.h"

#include "renderer/buffers.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_array.h"
#include "renderer/window.h"

#include "main/lunar_sprites.h"

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

#define FONT_BATCH_VBO_LAYOUT_COUNT 3
static const BufferElement FONT_BATCH_VBO_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT2, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coords", false },
	{ SHADER_DATA_TYPE_FLOAT4, "color", false },
};
#define FONT_BATCH_VBO_ROW_COUNT (2 + 2 + 1 + 4)
#define FONT_BATCH_VBO_ROW_SIZE (FONT_BATCH_VBO_ROW_COUNT * sizeof(float32))

#define FONT_BATCH_VBO_MAX_SIZE 1024 * FONT_BATCH_VBO_ROW_SIZE

extern const char *const FONT_SHADER_SOURCE;

typedef struct {
	int32 z_index;

	Color font_color;

	float32 vbo_data[FONT_BATCH_VBO_MAX_SIZE];
	size_t vbo_size;

	VertexBuffer *vbo;
	VertexArray *vao;

	Texture *atlas;
	Shader *shader;

	FontRenderTextCallback callback;
	void *user_data;

} FontRenderer;

static FontRenderer *font_renderer = NULL;

static void font_renderer_flush();

static void event_handler(Event *event, void *user_data) {
	if (event->type != EVENT_WINDOW) {
		return;
	}

	EventWindow window_event = event->window;

	if (window_event.type == EVENT_WINDOW_RESIZE) {
		RFont_update_framebuffer(window_event.size.x, window_event.size.y);
	}
}

static void font_on_update(float64 delta_time) {
	font_renderer_flush();
}

void font_renderer_init(Renderer *renderer, LSCore *core, const LSWindow *window) {
	font_renderer = ls_malloc(sizeof(FontRenderer));

	EventManager *event_manager = core_get_event_manager(core);
	event_manager_add_handler(event_manager, event_handler, NULL);

	Vector2u size = window_get_size(window);
	RFont_init(size.x, size.y);
	font_renderer->z_index = 0;
	font_renderer->vbo_size = 0;
	font_renderer->font_color = COLOR_WHITE;
	font_renderer->atlas = NULL;
	font_renderer->callback = NULL;
	font_renderer->user_data = NULL;

	font_renderer->shader = renderer_create_shader(renderer, FONT_SHADER_SOURCE, ls_str_length(FONT_SHADER_SOURCE));

	BufferLayout *vbo_layout = buffer_layout_create(FONT_BATCH_VBO_LAYOUT, FONT_BATCH_VBO_LAYOUT_COUNT);
	font_renderer->vbo = renderer_create_vertex_buffer(renderer, NULL, FONT_BATCH_VBO_MAX_SIZE, BUFFER_USAGE_DYNAMIC);
	vertex_buffer_set_layout(font_renderer->vbo, vbo_layout);

	font_renderer->vao = renderer_create_vertex_array(renderer);

	// Called at the end of the frame so we can accumulate all the text to render during update.
	ls_register_update_callback(font_on_update, true);
}

void font_renderer_deinit() {
	ls_free(font_renderer);
	RFont_close();
}

void font_set_render_color(Color color) {
	LS_ASSERT(font_renderer);

	font_renderer->font_color = color;
}

Vector2u font_draw_text(const Font *font, uint32 font_size, String text, float32 x, float32 y) {
	LS_ASSERT(font_renderer);

	RFont_area size = RFont_draw_text(font->font, text, x, y, font_size);
	return vec2u(size.w, size.h);
}

Vector2u font_draw_text_callback(const Font *font, uint32 font_size, String text, float32 x, float32 y, FontRenderTextCallback callback, void *user_data) {
	LS_ASSERT(font_renderer);

	font_renderer->callback = callback;
	font_renderer->user_data = user_data;
	RFont_area size = RFont_draw_text(font->font, text, x, y, font_size);
	font_renderer->callback = NULL;
	font_renderer->user_data = NULL;

	return vec2u(size.w, size.h);
}

Vector2u font_get_text_size(const Font *font, uint32 font_size, String text) {
	LS_ASSERT(font_renderer);

	RFont_area size = RFont_text_area(font->font, text, font_size);
	return vec2u(size.w, size.h);
}

static void font_renderer_flush() {
	if (font_renderer->vbo_size == 0) {
		return;
	}
	LS_ASSERT(font_renderer->atlas);

	vertex_array_bind(font_renderer->vao);
	vertex_buffer_set_sub_data(font_renderer->vbo, font_renderer->vbo_data, font_renderer->vbo_size * sizeof(float32), 0);

	const VertexBuffer *buffers[1] = { font_renderer->vbo };
	vertex_array_set_vertex_buffers(font_renderer->vao, buffers, 1);

	texture_bind(font_renderer->atlas, 0);
	shader_bind(font_renderer->shader);

	vertex_array_draw(font_renderer->vao);

	texture_unbind(font_renderer->atlas);
	shader_unbind(font_renderer->shader);

	font_renderer->vbo_size = 0;
}

void font_renderer_render_text(Texture *atlas, float32 *vertices, float32 *tcoords, size_t nverts) {
	LS_ASSERT(font_renderer);

	if (font_renderer->callback) {
		font_renderer->callback(atlas, vertices, tcoords, nverts, font_renderer->user_data);
		return;
	}

	size_t current_vbo_size = font_renderer->vbo_size * sizeof(float32);
	size_t draw_size = nverts * FONT_BATCH_VBO_ROW_SIZE;
	if (draw_size + current_vbo_size > FONT_BATCH_VBO_MAX_SIZE ||
			atlas != font_renderer->atlas) {
		font_renderer_flush();
	}

	font_renderer->atlas = atlas;

	for (size_t i = 0; i < nverts; i++) {
		font_renderer->vbo_data[font_renderer->vbo_size++] = vertices[i * 3];
		font_renderer->vbo_data[font_renderer->vbo_size++] = vertices[i * 3 + 1];

		font_renderer->vbo_data[font_renderer->vbo_size++] = tcoords[i * 2];
		font_renderer->vbo_data[font_renderer->vbo_size++] = tcoords[i * 2 + 1];

		font_renderer->vbo_data[font_renderer->vbo_size++] = font_renderer->font_color.r;
		font_renderer->vbo_data[font_renderer->vbo_size++] = font_renderer->font_color.g;
		font_renderer->vbo_data[font_renderer->vbo_size++] = font_renderer->font_color.b;
		font_renderer->vbo_data[font_renderer->vbo_size++] = font_renderer->font_color.a;
	}
}

_FORCE_INLINE_ Texture *font_renderer_create_atlas(uint32 atlas_width, uint32 atlas_height) {
	LS_ASSERT(font_renderer);

	uint8 *atlas_data = ls_calloc(atlas_width * atlas_height, sizeof(uint8));
	Texture *atlas = texture_create(atlas_width, atlas_height, TEXTURE_FORMAT_R, atlas_data);
	ls_free(atlas_data);
	LS_ASSERT(atlas);

	return atlas;
}

_FORCE_INLINE_ void font_renderer_bitmap_to_atlas(Texture *atlas, uint8 *bitmap, float32 width, float32 height, float32 x, float32 y) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(atlas);

	texture_add_sub_texture(atlas, TEXTURE_FORMAT_R, bitmap, x, y, width, height);
}

_FORCE_INLINE_ void font_renderer_free_atlas(Texture *atlas) {
	LS_ASSERT(font_renderer);

	texture_destroy(atlas);
}

_FORCE_INLINE_ uint8 font_renderer_resize(Texture **atlas, uint32 width, uint32 height) {
	LS_ASSERT(font_renderer);

	// TODO: Implement

	return 0;
}

// RFont implementation

#define RFONT_IMPLEMENTATION
#include <RFont.h>

void RFont_render_set_color(float r, float g, float b, float a) {
	LS_ASSERT_MSG(false, "%s", "Color setting is not supported in this context.");
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

uint8 RFont_resize_atlas(Texture **atlas, uint32 width, uint32 height) {
	return font_renderer_resize(atlas, width, height);
}

void RFont_render_free(Texture *atlas) {
	font_renderer_free_atlas(atlas);
}

void RFont_render_legacy(uint8 _) {
	LS_ASSERT_MSG(false, "%s", "Legacy rendering is not supported in this context.");
}

void RFont_render_init() {
}