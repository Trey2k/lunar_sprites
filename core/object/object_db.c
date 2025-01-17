#include "object_db.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/object/object.h"
#include "core/types/hashtable.h"
#include "core/types/string.h"
#include "core/types/variant.h"

typedef struct {
	char *name;
	ObjectMethod getter;
	ObjectMethod setter;
} ObjectProperty;

typedef struct {
	char *type_name;
	ObjectCreateFunction create;
	ObjectDestroyFunction destroy;
	ObjectDrawFunction draw;

	Hashtable *properties;
	Hashtable *methods;
} ObjectType;

typedef struct {
	Hashtable *types;

	ObjectType *active_type;
} ObjectDB;

static ObjectDB object_db = { 0 };

static void clear_types();

static void register_base_methods();

void object_db_init() {
	object_db.types = hashtable_create(HASHTABLE_KEY_UINT32, 16, false);
}

void object_db_deinit() {
	clear_types();
	hashtable_destroy(object_db.types);
}

void *object_db_create_instance(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->create();
}

void object_db_destroy_instance(uint32 type_id, void *object_data) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return;
	}

	type->destroy(object_data);
}

ObjectDrawFunction object_db_get_draw_function(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->draw;
}

uint32 object_db_register_type(String type_name, ObjectInterface interface) {
	ObjectType *type = ls_malloc(sizeof(ObjectType));
	type->type_name = ls_str_copy(type_name);
	type->create = interface.create;
	type->destroy = interface.destroy;
	type->draw = interface.draw;
	type->properties = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
	type->methods = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	uint32 type_id = object_db_get_type_id(type_name);

	hashtable_set(object_db.types, HASH_KEY(u32, type_id), HASH_VAL(ptr, type));

	object_db.active_type = type;
	register_base_methods();
	interface.register_methods();
	object_db.active_type = NULL;

	return type_id;
}

void object_db_register_property(String name, String getter, String setter) {
	LS_ASSERT(object_db.active_type);

	if (!hashtable_contains(object_db.active_type->methods, HASH_KEY(str, getter))) {
		ls_log(LOG_LEVEL_ERROR, "Getter method '%s' not found for property '%s'.\n", getter, name);
		return;
	}

	if (!hashtable_contains(object_db.active_type->methods, HASH_KEY(str, setter))) {
		ls_log(LOG_LEVEL_ERROR, "Setter method '%s' not found for property '%s'.\n", setter, name);
		return;
	}

	ObjectProperty *property = ls_malloc(sizeof(ObjectProperty));
	property->name = ls_str_copy(name);
	property->getter = (ObjectMethod)hashtable_get(object_db.active_type->methods, HASH_KEY(str, getter)).ptr;
	property->setter = (ObjectMethod)hashtable_get(object_db.active_type->methods, HASH_KEY(str, setter)).ptr;

	hashtable_set(object_db.active_type->properties, HASH_KEY(str, name), HASH_VAL(ptr, property));
}

void object_db_register_method(String name, ObjectMethod method) {
	LS_ASSERT(object_db.active_type);

	hashtable_set(object_db.active_type->methods, HASH_KEY(str, name), HASH_VAL(ptr, method));
}

uint32 object_db_get_type_id(String type_name) {
	return ls_str_hash_djb2(type_name);
}

String object_db_get_type_name(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->type_name;
}

bool object_db_type_has_property(uint32 type_id, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->properties, HASH_KEY(str, name));
}

ObjectPropertyInterface object_db_type_get_property(uint32 type_id, String name) {
	ObjectPropertyInterface interface = { 0 };

	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return interface;
	}

	ObjectProperty *property = hashtable_get(type->properties, HASH_KEY(str, name)).ptr;
	if (!property) {
		return interface;
	}

	interface.getter = property->getter;
	interface.setter = property->setter;

	return interface;
}

bool object_db_type_has_method(uint32 type_id, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->methods, HASH_KEY(str, name));
}

ObjectMethod object_db_type_get_method(uint32 type_id, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return hashtable_get(type->methods, HASH_KEY(str, name)).ptr;
}

static void clear_properties(Hashtable *properties) {
	Slice64 *keys = hashtable_get_keys(properties);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		String key = *(char **)slice64_get(keys, i).ptr;
		ObjectProperty *property = hashtable_get(properties, HASH_KEY(str, key)).ptr;
		ls_free(property->name);
		ls_free(property);
	}

	slice64_destroy(keys);
}

static void clear_types() {
	Slice64 *type_ids = hashtable_get_keys(object_db.types);
	for (size_t i = 0; i < slice64_get_size(type_ids); i++) {
		uint32 type_id = *(uint32 *)slice64_get(type_ids, i).ptr;
		ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
		ls_free(type->type_name);
		clear_properties(type->properties);
		hashtable_destroy(type->properties);
		hashtable_destroy(type->methods);
		ls_free(type);
	}

	slice64_destroy(type_ids);
}

static Variant db_object_add_child(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_OBJECT) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected object got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	object_add_child(object, args[0].OBJECT);

	return VARIANT_NIL;
}

static Variant db_object_get_child(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_INT) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected int got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	return VARIANT_OBJECT(object_get_child(object, args[0].INT));
}

static Variant db_object_get_child_count(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	return VARIANT_INT(object_get_child_count(object));
}

static Variant db_object_get_parent(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	Object *parent = object_get_parent(object);
	if (!parent) {
		return VARIANT_NIL;
	}

	return VARIANT_OBJECT(parent);
}

static Variant db_objet_draw(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_FLOAT) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected float got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	object_draw(object, args[0].FLOAT);

	return VARIANT_NIL;
}

static Variant db_object_set_position(Object *objecct, const Variant *args, size_t n_args) {
	if (n_args != 1) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 1 got %d\n", n_args);
		return VARIANT_NIL;
	}

	if (args[0].type != VARIANT_TYPE_VECTOR2I) {
		ls_log(LOG_LEVEL_ERROR, "Invalid argument type, expected vector2i got %s\n", variant_type_to_string(args[0].type));
		return VARIANT_NIL;
	}

	object_set_position(objecct, args[0].VECTOR2I);

	return VARIANT_NIL;
}

static Variant db_object_get_position(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR2I(object_get_position(object));
}

static Variant db_object_get_global_position(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR2I(object_get_global_position(object));
}

static void register_base_methods() {
	object_db_register_method("add_child", db_object_add_child);
	object_db_register_method("get_child", db_object_get_child);
	object_db_register_method("get_child_count", db_object_get_child_count);
	object_db_register_method("get_parent", db_object_get_parent);
	object_db_register_method("draw", db_objet_draw);
	object_db_register_method("set_position", db_object_set_position);
	object_db_register_method("get_position", db_object_get_position);
	object_db_register_method("get_global_position", db_object_get_global_position);

	object_db_register_property("position", "get_position", "set_position");
}