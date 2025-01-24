#include "core/object/object.h"
#include "core/debug.h"
#include "core/object/object_db.h"
#include "core/types/slice.h"
#include "core/types/variant.h"

#include <stdarg.h>

struct Object {
	uint32 type_id;
	void *data;

	Vector2i position;

	Slice64 *children;
	Object *parent;
};

Object *object_create(String type_name) {
	Object *object = ls_malloc(sizeof(Object));
	object->type_id = object_db_get_type_id(type_name);

	object->data = object_db_create_instance(object->type_id);

	object->children = slice64_create(16, true);
	object->parent = NULL;

	return object;
}

Object *object_create_from_data(String type_name, void *data) {
	Object *object = ls_malloc(sizeof(Object));
	object->type_id = object_db_get_type_id(type_name);

	object->data = data;

	object->children = slice64_create(16, true);
	object->parent = NULL;

	return object;
}

void object_destroy(Object *object) {
	object_db_destroy_instance(object->type_id, object->data);
	slice64_destroy(object->children);
	ls_free(object);
}

void object_draw(Object *object, float64 delta_time) {
	object_db_get_draw_function(object->type_id)(object, delta_time);
}

bool object_has_property(Object *object, String name) {
	return object_db_type_has_property(object->type_id, name);
}

Variant object_get_property(Object *object, String name) {
	ObjectPropertyInterface interface = object_db_type_get_property(object->type_id, name);
	if (!interface.getter) {
		ls_log(LOG_LEVEL_ERROR, "Object type %d does not have a property %s\n", object->type_id, name);
		return VARIANT_NIL;
	}

	return interface.getter(object, NULL, 0);
}

void object_set_property(Object *object, String name, Variant value) {
	ObjectPropertyInterface interface = object_db_type_get_property(object->type_id, name);
	if (!interface.setter) {
		ls_log(LOG_LEVEL_ERROR, "Object type %d does not have a property %s\n", object->type_id, name);
		return;
	}

	interface.setter(object, &value, 1);
}

bool object_has_method(Object *object, String name) {
	return object_db_type_has_method(object->type_id, name);
}

Variant object_call_method(Object *object, String name, Variant *args, size_t n_args) {
	ObjectMethod method = object_db_type_get_method(object->type_id, name);
	if (!method) {
		ls_log(LOG_LEVEL_ERROR, "Object type %d does not have a method %s\n", object->type_id, name);
		return VARIANT_NIL;
	}

	return method(object, args, n_args);
}

void object_set_position(Object *object, Vector2i position) {
	LS_ASSERT(object);

	object->position = position;
}

Vector2i object_get_position(Object *object) {
	LS_ASSERT(object);

	return object->position;
}

Vector2i object_get_global_position(Object *object) {
	LS_ASSERT(object);

	Vector2i position = object->position;
	Object *parent = object->parent;

	while (parent) {
		position = vec2i_add(position, parent->position);
		parent = parent->parent;
	}

	return position;
}

void object_add_child(Object *parent, Object *child) {
	LS_ASSERT(parent);
	LS_ASSERT(child);

	slice64_append(parent->children, SLICE_VAL64(ptr, child));
	child->parent = parent;
}

void object_remove_child(Object *parent, Object *child) {
	LS_ASSERT(parent);
	LS_ASSERT(child);

	for (size_t i = 0; i < slice64_get_size(parent->children); i++) {
		if (slice64_get(parent->children, i).ptr == child) {
			slice64_remove(parent->children, i);
			child->parent = NULL;
			return;
		}
	}
}

int32 object_get_child_count(Object *object) {
	LS_ASSERT(object);

	size_t r = slice64_get_size(object->children);

	return (int32)r;
}

Object *object_get_child(Object *object, int32 index) {
	LS_ASSERT(object);

	LS_ASSERT(index >= 0 && index < slice64_get_size(object->children));

	return slice64_get(object->children, (size_t)index).ptr;
}

Object *object_get_parent(Object *object) {
	LS_ASSERT(object);

	return object->parent;
}

bool object_equals(Object *a, Object *b) {
	// TODO: Maybe instance ID?
	return a == b;
}

void *object_get_data(Object *object) {
	LS_ASSERT(object);

	return object->data;
}

uint32 object_get_type_id(Object *object) {
	LS_ASSERT(object);

	return object->type_id;
}

String object_get_type_name(Object *object) {
	LS_ASSERT(object);

	return object_db_get_type_name(object->type_id);
}