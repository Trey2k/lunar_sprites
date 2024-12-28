#include "font_renderer.h"

#include "core/debug.h"
#include "core/events/events.h"
#include "core/memory.h"

#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
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
	Color color;
	bool ready;
} FontRenderer;

static FontRenderer *font_renderer = NULL;

static void font_renderer_start(const LSWindow *window) {
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
	font_renderer->color = COLOR_WHITE;

	font_renderer->ready = false;
	EventManager *event_manager = core_get_event_manager(core);
	event_manager_add_handler(event_manager, event_handler, NULL);
}

void font_renderer_deinit() {
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
	batch_renderer_draw_basic(atlas, vertices, tcoords, font_renderer->color, nverts);
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