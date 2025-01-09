#include "font.h"

#include "core/debug.h"
#include "core/events/events.h"
#include "core/memory.h"
#include "core/types/color.h"

#include "renderer/batch_renderer.h"
#include "renderer/texture.h"
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

extern const char *const FONT_SHADER_SOURCE;

typedef struct {
	Color font_color;

	Slice32 *indices;
	BatchVertex *batch_vertices;
	size_t batch_vertices_size;

	FontRenderTextCallback callback;
	void *user_data;
} FontRenderer;

static FontRenderer *font_renderer = NULL;

static void event_handler(Event *event, void *user_data) {
	if (event->type != EVENT_WINDOW) {
		return;
	}

	EventWindow window_event = event->window;

	if (window_event.type == EVENT_WINDOW_RESIZE) {
		RFont_update_framebuffer(window_event.size.x, window_event.size.y);
	}
}

void font_renderer_init(Renderer *renderer, LSCore *core, const LSWindow *window) {
	font_renderer = ls_malloc(sizeof(FontRenderer));

	EventManager *event_manager = core_get_event_manager(core);
	event_manager_add_handler(event_manager, event_handler, NULL);

	Vector2u size = window_get_size(window);
	RFont_init(size.x, size.y);
	font_renderer->font_color = COLOR_WHITE;
	font_renderer->callback = NULL;
	font_renderer->user_data = NULL;

	font_renderer->indices = slice32_create(128);
	font_renderer->batch_vertices = ls_malloc(128 * sizeof(BatchVertex));
	font_renderer->batch_vertices_size = 128;
}

void font_renderer_deinit() {
	ls_free(font_renderer);
	RFont_close();
}

Vector2u font_draw_text(const Font *font, uint32 font_size, Color font_color, String text, Vector2 position) {
	LS_ASSERT(font_renderer);

	font_renderer->font_color = font_color;

	RFont_area size = RFont_draw_text(font->font, text, position.x, position.y, font_size);
	return vec2u(size.w, size.h);
}

Vector2u font_get_text_size(const Font *font, uint32 font_size, String text) {
	LS_ASSERT(font_renderer);

	RFont_area size = RFont_text_area(font->font, text, font_size);
	return vec2u(size.w, size.h);
}

void font_renderer_render_text(Texture *atlas, float32 *vertices, float32 *tcoords, size_t nverts) {
	LS_ASSERT(font_renderer);

	if (font_renderer->callback) {
		font_renderer->callback(atlas, vertices, tcoords, nverts, font_renderer->user_data);
		return;
	}

	slice32_clear(font_renderer->indices);
	if (font_renderer->batch_vertices_size < nverts) {
		font_renderer->batch_vertices = ls_realloc(font_renderer->batch_vertices, nverts * sizeof(BatchVertex));
		font_renderer->batch_vertices_size = nverts;
	}

	for (size_t i = 0; i < nverts; i++) {
		BatchVertex *vertex = &font_renderer->batch_vertices[i];
		vertex->pos = vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
		vertex->tex_coords = vec2(tcoords[i * 2], tcoords[i * 2 + 1]);
		vertex->color = font_renderer->font_color;
		vertex->element_size = vec2(0.0f, 0.0f);
		vertex->radius = 0.0f;

		slice32_append(font_renderer->indices, SLICE_VAL32(u32, (uint32)i));
	}

	batch_renderer_draw(atlas, font_renderer->batch_vertices, slice32_get_data(font_renderer->indices), nverts, nverts);
}

_FORCE_INLINE_ Texture *font_renderer_create_atlas(uint32 atlas_width, uint32 atlas_height) {
	LS_ASSERT(font_renderer);

	uint8 *atlas_data = ls_calloc(atlas_width * atlas_height * 4, sizeof(uint8));
	Texture *atlas = texture_create(atlas_width, atlas_height, TEXTURE_FORMAT_RGBA, atlas_data);
	ls_free(atlas_data);
	LS_ASSERT(atlas);

	return atlas;
}

_FORCE_INLINE_ void font_renderer_bitmap_to_atlas(Texture *atlas, uint8 *bitmap, float32 width, float32 height, float32 x, float32 y) {
	LS_ASSERT(font_renderer);
	LS_ASSERT(atlas);

	// WebGL does not support rgba swizzling, so we need to uncompressed the bitmap data.

	uint8 *uncompressed_bitmap = ls_malloc(width * height * 4 * sizeof(uint8));
	for (size_t i = 0; i < width * height; i++) {
		uncompressed_bitmap[i * 4] = 255;
		uncompressed_bitmap[i * 4 + 1] = 255;
		uncompressed_bitmap[i * 4 + 2] = 255;
		uncompressed_bitmap[i * 4 + 3] = bitmap[i];
	}

	texture_add_sub_texture(atlas, TEXTURE_FORMAT_RGBA, uncompressed_bitmap, x, y, width, height);

	ls_free(uncompressed_bitmap);
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