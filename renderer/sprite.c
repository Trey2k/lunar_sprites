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

	const Renderer *renderer;

	Vector2i viewport_size;
};

static float32 *spriate_get_transformed_vertices(const Sprite *sprite) {
	float32 *vertices = ls_malloc(sizeof(float32) * SPRITE_VERTICIES_COUNT * 3);

	for (size_t i = 0; i < SPRITE_VERTICIES_COUNT; i++) {
		Vector3 vertex = vec3(SPRITE_VERTICIES[i * 3], SPRITE_VERTICIES[i * 3 + 1], SPRITE_VERTICIES[i * 3 + 2]);
		vertex = mat4_multiply_vec3(sprite->transform, vertex);

		vertices[i * 3] = vertex.x;
		vertices[i * 3 + 1] = vertex.y;
		vertices[i * 3 + 2] = vertex.z;
	}

	return vertices;
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

void sprite_draw(const Sprite *sprite) {
	batch_renderer_draw(NULL, sprite->texture, spriate_get_transformed_vertices(sprite), SPRITE_TEX_COORDS, SPRITE_INDECIES, COLOR_WHITE, SPRITE_VERTICIES_COUNT, SPRITE_INDECIES_COUNT);
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