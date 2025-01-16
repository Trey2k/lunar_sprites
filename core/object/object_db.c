#include "object_db.h"

#include "core/core.h"
#include "core/types/hashtable.h"

typedef struct {
	char *name;
	ObjectMethod getter;
	ObjectMethod setter;
} ObjectProperty;

typedef struct {
	char *type_name;
	ObjectCreateFunction create;
	ObjectDestroyFunction destroy;

	Hashtable *properties;
	Hashtable *methods;
} ObjectType;

typedef struct {
	Hashtable *types;

	ObjectType *active_type;
} ObjectDB;

static ObjectDB object_db = { 0 };

static void clear_types();

void object_db_init() {
	object_db.types = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
}

void object_db_deinit() {
	clear_types();
	hashtable_destroy(object_db.types);
}

void object_db_register_type(String type_name, ObjectCreateFunction create, ObjectDestroyFunction destroy, ObjectRegisterMethodsFunction register_methods) {
	ObjectType *type = ls_malloc(sizeof(ObjectType));
	type->type_name = ls_str_copy(type_name);
	type->create = create;
	type->destroy = destroy;
	type->properties = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
	type->methods = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	hashtable_set(object_db.types, HASH_KEY(str, type_name), HASH_VAL(ptr, type));

	object_db.active_type = type;
	register_methods();
	object_db.active_type = NULL;
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
}

void object_db_register_method(String name, ObjectMethod method) {
	LS_ASSERT(object_db.active_type);

	hashtable_set(object_db.active_type->methods, HASH_KEY(str, name), HASH_VAL(ptr, method));
}

static void clear_properties(Hashtable *properties) {
	Slice *keys = hashtable_get_keys(properties);
	for (size_t i = 0; i < slice_get_size(keys); i++) {
		String key = slice_get(keys, i).str;
		ObjectProperty *property = hashtable_get(properties, HASH_KEY(str, key)).ptr;
		ls_free(property->name);
		ls_free(property);
	}

	slice_destroy(keys);
}

static void clear_types() {
	Slice *type_names = hashtable_get_keys(object_db.types);
	for (size_t i = 0; i < slice_get_size(type_names); i++) {
		String type_name = slice_get(type_names, i).str;
		ObjectType *type = hashtable_get(object_db.types, HASH_KEY(str, type_name)).ptr;
		ls_free(type->type_name);
		clear_properties(type->properties);
		hashtable_destroy(type->properties);
		ls_free(type);
	}

	slice_destroy(type_names);
}