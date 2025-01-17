#include "core/resource/resource_db.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/types/hashtable.h"
#include "core/types/string.h"

typedef struct {
	char *name;
	ResourceMethod getter;
	ResourceMethod setter;
} ResourceProperty;

typedef struct {
	char *type_name;
	char *extensions;
	ResourceCreateFunction create;
	ResourceDestroyFunction destroy;

	Hashtable *properties;
	Hashtable *methods;
} ResourceType;

typedef struct {
	Hashtable *types;

	ResourceType *active_type;
} ResourceDB;

static ResourceDB resource_db = { 0 };

static void clear_types();

void resource_db_init() {
	resource_db.types = hashtable_create(HASHTABLE_KEY_UINT32, 16, false);
}

void resource_db_deinit() {
	clear_types();
	hashtable_destroy(resource_db.types);
}

void *resource_db_create_instance(uint32 type_id, String path) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return NULL;
	}

	return type->create(path);
}

void resource_db_destroy_instance(uint32 type_id, void *resource_data) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return;
	}

	type->destroy(resource_data);
}

uint32 resource_db_register_type(String type_name, String extensions, ResourceCreateFunction create, ResourceDestroyFunction destroy, ResourceRegisterMethodsFunction register_methods) {
	ResourceType *type = ls_malloc(sizeof(ResourceType));
	type->type_name = ls_str_copy(type_name);
	type->extensions = ls_str_copy(extensions);
	type->create = create;
	type->destroy = destroy;
	type->properties = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
	type->methods = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	uint32 type_id = ls_str_hash_djb2(type_name);
	hashtable_set(resource_db.types, HASH_KEY(u32, type_id), HASH_VAL(ptr, type));

	resource_db.active_type = type;
	register_methods();
	resource_db.active_type = NULL;

	return type_id;
}

void resource_db_register_property(String name, String getter, String setter) {
	LS_ASSERT(resource_db.active_type);

	ResourceProperty *property = ls_malloc(sizeof(ResourceProperty));
	property->name = ls_str_copy(name);
	property->getter = (ResourceMethod)hashtable_get(resource_db.active_type->methods, HASH_KEY(str, getter)).ptr;
	property->setter = (ResourceMethod)hashtable_get(resource_db.active_type->methods, HASH_KEY(str, setter)).ptr;

	hashtable_set(resource_db.active_type->properties, HASH_KEY(str, name), HASH_VAL(ptr, property));
}

void resource_db_register_method(String name, ResourceMethod method) {
	LS_ASSERT(resource_db.active_type);

	hashtable_set(resource_db.active_type->methods, HASH_KEY(str, name), HASH_VAL(ptr, method));
}

uint32 resource_db_get_type_id(String type_name) {
	return ls_str_hash_djb2(type_name);
}

uint32 resource_db_get_type_id_by_extension(String extension) {
	Slice64 *keys = hashtable_get_keys(resource_db.types);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		uint32 type_id = *(uint32 *)slice64_get(keys, i).ptr;
		ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
		if (ls_str_contains(type->extensions, extension)) {
			slice64_destroy(keys);
			return type_id;
		}
	}

	slice64_destroy(keys);
	return 0;
}

bool resource_db_type_has_property(uint32 type_id, String name) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->properties, HASH_KEY(str, name));
}

ResourcePropertyInterface resource_db_type_get_property(uint32 type_id, String name) {
	ResourcePropertyInterface interface = { 0 };

	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return interface;
	}

	ResourceProperty *property = hashtable_get(type->properties, HASH_KEY(str, name)).ptr;
	if (!property) {
		return interface;
	}

	interface.getter = property->getter;
	interface.setter = property->setter;

	return interface;
}

bool resource_db_type_has_method(uint32 type_id, String name) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->methods, HASH_KEY(str, name));
}

ResourceMethod resource_db_type_get_method(uint32 type_id, String name) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
		return NULL;
	}

	return hashtable_get(type->methods, HASH_KEY(str, name)).ptr;
}

static void clear_properties(Hashtable *properties) {
	Slice64 *keys = hashtable_get_keys(properties);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		String key = slice64_get(keys, i).str;
		ResourceProperty *property = hashtable_get(properties, HASH_KEY(str, key)).ptr;
		ls_free(property->name);
		ls_free(property);
	}

	slice64_destroy(keys);
}

static void clear_types() {
	Slice64 *keys = hashtable_get_keys(resource_db.types);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		uint32 key = *(uint32 *)slice64_get(keys, i).ptr;
		ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, key)).ptr;
		ls_free(type->type_name);
		ls_free(type->extensions);
		clear_properties(type->properties);
		hashtable_destroy(type->properties);
		hashtable_destroy(type->methods);
		ls_free(type);
	}

	slice64_destroy(keys);
}