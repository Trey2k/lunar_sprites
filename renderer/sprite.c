#include "renderer/sprite.h"

#include "renderer/batch_renderer.h"
#include "renderer/texture.h"

#define SPRITE_VERTICIES_COUNT 4
static const float32 SPRITE_VERTICIES[] = {
	// positions          // texture coords
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f
};

static const float32 SPRITE_TEX_COORDS[] = {
	1.0f, 1.0f, // top right
	1.0f, 0.0f, // bottom right
	0.0f, 0.0f, // bottom left
	0.0f, 1.0f // top left
};

#define SPRITE_INDECIES_COUNT 6
static const uint32 SPRITE_INDECIES[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

struct Sprite {
	Vector2 size;
	Matrix4 transform;

	Texture *texture;
	BatchVertex vertices[SPRITE_VERTICIES_COUNT];

	const Renderer *renderer;

	Vector2i viewport_size;
};

static void spriate_transform_vertices(Sprite *sprite) {
	for (size_t i = 0; i < SPRITE_VERTICIES_COUNT; i++) {
		sprite->vertices[i].pos = mat4_multiply_vec3(sprite->transform, vec3(SPRITE_VERTICIES[i * 3], SPRITE_VERTICIES[i * 3 + 1], SPRITE_VERTICIES[i * 3 + 2]));
		sprite->vertices[i].tex_coords = vec2(SPRITE_TEX_COORDS[i * 2], SPRITE_TEX_COORDS[i * 2 + 1]);
		sprite->vertices[i].color = COLOR_WHITE;
		sprite->vertices[i].element_size = vec2(sprite->size.x, sprite->size.y);
		sprite->vertices[i].radius = 0.0f;
	}
}

Sprite *renderer_create_sprite(const Renderer *renderer, String image_path, Vector2 position, Vector2 scale, float32 rotation) {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->renderer = renderer;
	sprite->texture = texture_create_from_image(image_path);

	sprite->size = vec2(texture_get_width(sprite->texture), texture_get_height(sprite->texture));
	sprite->size = vec2(sprite->size.x * scale.x, sprite->size.y * scale.y);

	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
	sprite->transform = mat4_multiply(sprite->transform, mat4_scale(vec3(scale.x, scale.y, 1.0)));

	return sprite;
}

Sprite *renderer_create_sprite_texture(const Renderer *renderer, Texture *texture, Vector2 position, Vector2 scale, float32 rotation) {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->renderer = renderer;
	sprite->texture = texture;

	sprite->size = vec2(texture_get_width(sprite->texture), texture_get_height(sprite->texture));
	sprite->size = vec2(sprite->size.x * scale.x, sprite->size.y * scale.y);

	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
	sprite->transform = mat4_multiply(sprite->transform, mat4_scale(vec3(scale.x, scale.y, 1.0)));

	return sprite;
}

void sprite_destroy(Sprite *sprite) {
	texture_destroy(sprite->texture);

	ls_free(sprite);
}

void sprite_draw(Sprite *sprite) {
	spriate_transform_vertices(sprite);
	batch_renderer_draw(sprite->texture, sprite->vertices, SPRITE_INDECIES, SPRITE_VERTICIES_COUNT, SPRITE_INDECIES_COUNT);
}

void sprite_set_position(Sprite *sprite, Vector2 position) {
	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
}

Vector2 sprite_get_position(const Sprite *sprite) {
	return vec2(sprite->transform.w0, sprite->transform.w1);
}

void sprite_set_scale(Sprite *sprite, Vector2 scale) {
	sprite->transform = mat4_scale(vec3(sprite->size.x * scale.x, sprite->size.y * scale.y, 1.0));
}

Vector2 sprite_get_scale(const Sprite *sprite) {
	return vec2(sprite->transform.x0, sprite->transform.y1);
}

void sprite_set_rotation(Sprite *sprite, float32 rotation) {
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
}

float32 sprite_get_rotation(const Sprite *sprite) {
	return sprite->transform.z2;
}