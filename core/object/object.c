#include "core/object/object.h"
#include "core/debug.h"
#include "core/memory.h"
#include "core/object/object_db.h"
#include "core/types/slice.h"
#include "core/types/variant.h"

#include <stdarg.h>

struct Object {
	uint64 type_id;
	void *data;

	size_t ref_count;

	Vector2i position;

	Slice64 *children;
	Object *parent;
};

Object *object_create(BString type_name) {
	Object *object = ls_malloc(sizeof(Object));
	object->type_id = object_db_get_type_id(type_name);

	object->data = object_db_create_instance(object->type_id);

	object->children = slice64_create(16, true);
	object->parent = NULL;
	object->ref_count = 1;

	return object;
}

Object *object_create_from_data(BString type_name, void *data) {
	Object *object = ls_malloc(sizeof(Object));
	object->type_id = object_db_get_type_id(type_name);

	object->data = data;

	object->children = slice64_create(16, true);
	object->parent = NULL;
	object->ref_count = 1;

	return object;
}

void object_ref(Object *object) {
	LS_ASSERT(object);

	object->ref_count++;
}

void object_unref(Object *object) {
	LS_ASSERT(object);

	object->ref_count--;

	if (object->ref_count != 0) {
		return;
	}

	for (size_t i = 0; i < slice64_get_size(object->children); i++) {
		object_unref(slice64_get(object->children, i).ptr);
	}

	object_db_destroy_instance(object->type_id, object->data);
	slice64_destroy(object->children);
	ls_free(object);
}

void object_draw(Object *object, float64 delta_time) {
	object_db_get_draw_function(object->type_id)(object, delta_time);
}

void object_update(Object *object, float64 delta_time) {
	object_db_get_update_function(object->type_id)(object, delta_time);
}

bool object_has_property(const Object *object, BString name) {
	return object_db_type_has_property(object->type_id, name);
}

Variant object_get_property(const Object *object, BString name) {
	return object_db_type_get_property(object->type_id, object, name);
}

VariantType object_get_property_type(const Object *object, BString name) {
	return object_db_type_get_property_type(object->type_id, name);
}

Array *object_get_properties(const Object *object) {
	return object_db_type_get_properties(object->type_id);
}

void object_set_property(Object *object, BString name, Variant value) {
	object_db_type_set_property(object->type_id, object, name, value);
}

bool object_has_method(const Object *object, BString name) {
	return object_db_type_has_method(object->type_id, name);
}

Variant object_call_method(Object *object, BString name, Variant *args, size_t n_args) {
	return object_db_type_call_method(object->type_id, object, name, args, n_args);
}

Array *object_get_methods(const Object *object) {
	return object_db_type_get_methods(object->type_id);
}

void object_set_position(Object *object, Vector2i position) {
	LS_ASSERT(object);

	object->position = position;
}

Vector2i object_get_position(const Object *object) {
	LS_ASSERT(object);

	return object->position;
}

Vector2i object_get_global_position(const Object *object) {
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

int32 object_get_child_count(const Object *object) {
	LS_ASSERT(object);

	size_t r = slice64_get_size(object->children);

	return (int32)r;
}

Object *object_get_child(const Object *object, int32 index) {
	LS_ASSERT(object);

	LS_ASSERT(index >= 0 && index < slice64_get_size(object->children));

	return slice64_get(object->children, (size_t)index).ptr;
}

Object *object_get_parent(const Object *object) {
	LS_ASSERT(object);

	return object->parent;
}

bool object_equals(const Object *a, const Object *b) {
	// TODO: Maybe instance ID?
	return a == b;
}

void *object_get_data(Object *object) {
	LS_ASSERT(object);

	return object->data;
}

const void *object_get_const_data(const Object *object) {
	LS_ASSERT(object);

	return object->data;
}

uint64 object_get_type_id(const Object *object) {
	LS_ASSERT(object);

	return object->type_id;
}

BString object_get_type_name(const Object *object) {
	LS_ASSERT(object);

	return object_db_get_type_name(object->type_id);
}

BString object_to_string(const Object *object) {
	LS_ASSERT(object);

	return object_db_get_to_string_function(object->type_id)(object);
}