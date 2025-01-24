#include "renderer/sprite.h"

#include "core/log.h"
#include "core/math/matrix.h"
#include "core/math/quaternion.h"
#include "core/math/vector.h"
#include "core/object/object.h"
#include "core/resource/resource.h"
#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"

#include "core/object/object_db.h"

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
	Vector2u size;
	Vector2 scale;
	Vector2i position;
	Matrix4 transform;

	float32 rotation;

	Resource *texture;
	BatchVertex vertices[SPRITE_VERTICIES_COUNT];
};

static void spriate_transform_vertices(Sprite *sprite) {
	Vector2 scale = sprite_get_scale(sprite);
	Vector2u viewport_size = renderer_get_viewport_size();

	for (size_t i = 0; i < SPRITE_VERTICIES_COUNT; i++) {
		Vector3 pos = vec3(SPRITE_VERTICIES[i * 3], SPRITE_VERTICIES[i * 3 + 1], SPRITE_VERTICIES[i * 3 + 2]);
		//	pos = quat_multiply_vec3(sprite->quat, pos);
		sprite->vertices[i].pos = mat4_multiply_vec3(sprite->transform, pos);

		//sprite->vertices[i].pos = mat4_multiply_vec3(sprite->rotation_transform, sprite->vertices[i].pos);
		// Scale from pixel space coordinates to 0 - 1
		sprite->vertices[i].pos.x = (sprite->vertices[i].pos.x / ((float32)viewport_size.x / 2)) - 1.0f;
		sprite->vertices[i].pos.y = 1.0f - (sprite->vertices[i].pos.y / ((float32)viewport_size.y / 2));

		sprite->vertices[i].tex_coords = vec2(SPRITE_TEX_COORDS[i * 2], SPRITE_TEX_COORDS[i * 2 + 1]);
		sprite->vertices[i].color = COLOR_WHITE;
		sprite->vertices[i].element_size = vec2(sprite->size.x * scale.x, sprite->size.y * scale.y);
		sprite->vertices[i].radius = 0.0f;
	}
}

static void update_transform(Sprite *sprite) {
	Vector3 scaled_size = vec3(sprite->scale.x * sprite->size.x, sprite->scale.y * sprite->size.x, 1.0);

	Quaternion quat = quat_rotate(QUAT_IDENTITY, sprite->rotation, vec3(0.0, 0.0, 1.0));
	quat = quat_normalize(quat);

	sprite->transform = MAT4_IDENTITY;
	sprite->transform = mat4_translate(sprite->transform, vec3(sprite->position.x, sprite->position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, quat_rotation_matrix(quat));
	sprite->transform = mat4_scale(sprite->transform, vec3(scaled_size.x, scaled_size.y, 1.0));
}

Sprite *sprite_create_empty() {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->texture = NULL;

	sprite->size = vec2u(0.0, 0.0);
	sprite->scale = vec2(1.0, 1.0);
	sprite->position = vec2i(0, 0);
	sprite->rotation = 0.0;

	update_transform(sprite);

	return sprite;
}

Sprite *sprite_create(String image_path, Vector2i position, Vector2 scale, float32 rotation) {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->texture = resource_create(image_path);

	Texture *texture = texture_from_resource(sprite->texture);

	sprite->size = vec2u(texture_get_width(texture), texture_get_height(texture));
	sprite->scale = scale;
	sprite->position = position;
	sprite->rotation = rotation;

	update_transform(sprite);

	return sprite;
}

void sprite_destroy(Sprite *sprite) {
	resource_destroy(sprite->texture);

	ls_free(sprite);
}

void sprite_draw(Sprite *sprite) {
	if (!sprite->texture) {
		ls_log(LOG_LEVEL_WARNING, "Sprites must have a texture to be drawn.\n");
		return;
	}

	spriate_transform_vertices(sprite);
	Texture *texture = texture_from_resource(sprite->texture);
	batch_renderer_draw(texture, sprite->vertices, SPRITE_INDECIES, SPRITE_VERTICIES_COUNT, SPRITE_INDECIES_COUNT);
}

void sprite_set_position(Sprite *sprite, Vector2i position) {
	sprite->position = position;

	update_transform(sprite);
}

Vector2i sprite_get_position(const Sprite *sprite) {
	return sprite->position;
}

void sprite_set_texture(Sprite *sprite, Resource *texture) {
	Texture *tex = texture_from_resource(texture);
	sprite->size = vec2u(texture_get_width(tex), texture_get_height(tex));
	sprite->texture = texture;

	update_transform(sprite);
}

Resource *sprite_get_texture(const Sprite *sprite) {
	return sprite->texture;
}

Vector2u sprite_get_size(const Sprite *sprite) {
	return sprite->size;
}

void sprite_set_scale(Sprite *sprite, Vector2 scale) {
	sprite->scale = scale;

	update_transform(sprite);
}

Vector2 sprite_get_scale(const Sprite *sprite) {
	return sprite->scale;
}

void sprite_set_rotation(Sprite *sprite, float32 rotation) {
	sprite->rotation = rotation;

	update_transform(sprite);
}

float32 sprite_get_rotation(const Sprite *sprite) {
	return sprite->rotation;
}