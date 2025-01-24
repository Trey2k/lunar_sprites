#include "sprite_object.h"
#include "core/math/vector.h"
#include "sprite.h"

#include "core/core.h"
#include "core/object/object_db.h"

static uint32 object_type_id = 0;

uint32 sprite_get_object_type() {
	return object_type_id;
}

bool object_is_sprite(Object *object) {
	return object_get_type_id(object) == object_type_id;
}

Sprite *object_to_sprite(Object *object) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return NULL;
	}

	return (Sprite *)object_get_data(object);
}

Object *sprite_to_object(Sprite *sprite) {
	return object_create_from_data(SPRITE_OBJECT_TYPE, sprite);
}

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

	return VARIANT_RESOURCE(sprite_get_texture(sprite));
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
	sprite_set_texture(sprite, args[0].RESOURCE);

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

	Vector2u size = sprite_get_size(sprite);

	return VARIANT_VECTOR2I(vec2i(size.x, size.y));
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

	sprite_set_rotation((Sprite *)object_get_data(object), (float32)args[0].FLOAT);

	return VARIANT_NIL;
}

static Variant sprite_object_set_scale(Object *object, const Variant *args, size_t n_args) {
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

	sprite_set_scale((Sprite *)object_get_data(object), args[0].VECTOR2);

	return VARIANT_NIL;
}

static Variant sprite_object_get_scale(Object *object, const Variant *args, size_t n_args) {
	if (object_get_type_id(object) != object_type_id) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object type, expected Sprite got %s\n", object_get_type_name(object));
		return VARIANT_NIL;
	}

	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	Sprite *sprite = (Sprite *)object_get_data(object);

	return VARIANT_VECTOR2(sprite_get_scale(sprite));
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

	object_db_register_method("set_scale", sprite_object_set_scale);
	object_db_register_method("get_scale", sprite_object_get_scale);

	object_db_register_property("texture", "get_texture", "set_texture");
	object_db_register_property("size", "get_size", "set_size");
	object_db_register_property("scale", "get_scale", "set_scale");
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