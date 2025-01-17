#include "renderer/sprite.h"

#include "core/log.h"
#include "core/math/matrix.h"
#include "core/object/object.h"
#include "renderer/batch_renderer.h"
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
	Matrix4 transform;

	Resource *texture;
	BatchVertex vertices[SPRITE_VERTICIES_COUNT];
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

Sprite *sprite_create_empty() {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->texture = NULL;

	sprite->size = vec2u(0.0, 0.0);
	sprite->transform = MAT4_IDENTITY;

	return sprite;
}

Sprite *sprite_create(String image_path, Vector2i position, Vector2 scale, float32 rotation) {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->texture = resource_create(image_path);

	Texture *texture = texture_from_resource(sprite->texture);

	sprite->size = vec2u(texture_get_width(texture), texture_get_height(texture));
	sprite->size = vec2u(sprite->size.x * scale.x, sprite->size.y * scale.y);

	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
	sprite->transform = mat4_multiply(sprite->transform, mat4_scale(vec3(scale.x, scale.y, 1.0)));

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
	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
}

Vector2i sprite_get_position(const Sprite *sprite) {
	return vec2i(sprite->transform.w0, sprite->transform.w1);
}

void sprite_scale(Sprite *sprite, Vector2 scale) {
	sprite->transform = mat4_scale(vec3(sprite->size.x * scale.x, sprite->size.y * scale.y, 1.0));
}

void sprite_set_rotation(Sprite *sprite, float32 rotation) {
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
}

float32 sprite_get_rotation(const Sprite *sprite) {
	return sprite->transform.z2;
}

static uint32 object_type_id = 0;

static Variant sprite_object_get_texture(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);

	return VARIANT_RESOURCE(sprite->texture);
}

static Variant sprite_object_set_texture(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_RESOURCE) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected resource got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);

	sprite->texture = args[0].RESOURCE;

	Texture *texture = texture_from_resource(sprite->texture);

	sprite->size = vec2u(texture_get_width(texture), texture_get_height(texture));
	sprite->size = vec2u(sprite->size.x, sprite->size.y);

	Vector2i position = object_get_global_position(object);
	float32 rotation = sprite_get_rotation(sprite);

	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));

	return VARIANT_NIL;
}

static Variant sprite_object_get_size(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);

	return VARIANT_VECTOR2(vec2(sprite->size.x, sprite->size.y));
}

static Variant sprite_object_set_size(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	ls_log(LOG_LEVEL_ERROR, "Sprite size is read only\n");

	return VARIANT_NIL;
}

static Variant sprite_object_get_rotation(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);

	return VARIANT_FLOAT((float64)sprite_get_rotation(sprite));
}

static Variant sprite_object_set_rotation(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_FLOAT) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected float got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	ls_log(LOG_LEVEL_INFO, "Setting rotation to %f\n", args[0].FLOAT);

	sprite_set_rotation((Sprite *)object_get_data(object), (float32)args[0].FLOAT);

	return VARIANT_NIL;
}

static Variant sprite_object_scale(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_VECTOR2) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected vector2 got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	sprite_scale((Sprite *)object_get_data(object), args[0].VECTOR2);

	return VARIANT_NIL;
}

static void *sprite_create_object() {
	return (void *)sprite_create_empty();
}

static void sprite_destroy_object(void *object_data) {
	sprite_destroy((Sprite *)object_data);
}

static void sprite_draw_object(Object *object, float64 delta_time) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);
	Vector2i position = object_get_global_position(object);
	sprite_set_position(sprite, position);

	sprite_draw(sprite);
}

static void sprite_register_methods() {
	object_db_register_method("get_texture", sprite_object_get_texture);
	object_db_register_method("set_texture", sprite_object_set_texture);

	object_db_register_method("get_size", sprite_object_get_size);
	object_db_register_method("set_size", sprite_object_set_size);

	object_db_register_method("get_rotation", sprite_object_get_rotation);
	object_db_register_method("set_rotation", sprite_object_set_rotation);

	object_db_register_method("scale", sprite_object_scale);

	object_db_register_property("texture", "get_texture", "set_texture");
	object_db_register_property("size", "get_size", "set_size");
	object_db_register_property("rotation", "get_rotation", "set_rotation");
}

void register_sprite_object() {
	object_type_id = object_db_register_type(SPRITE_OBJECT_TYPE,
			(ObjectInterface){
					.create = sprite_create_object,
					.destroy = sprite_destroy_object,
					.register_methods = sprite_register_methods,
					.draw = sprite_draw_object });
}