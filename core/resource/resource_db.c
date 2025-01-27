#include "core/resource/resource_db.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/hashtable.h"
#include "core/types/string.h"

typedef struct {
	char *type_name;
	char *extensions;
	ResourceCreateFunction create;
	ResourceDestroyFunction destroy;
} ResourceType;

typedef struct {
	Hashtable *types;
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

uint32 resource_db_register_type(BString type_name, String extensions, ResourceCreateFunction create, ResourceDestroyFunction destroy) {
	ResourceType *type = ls_malloc(sizeof(ResourceType));
	type->type_name = bstring_encode_utf8(type_name);
	type->extensions = ls_str_copy(extensions);
	type->create = create;
	type->destroy = destroy;

	uint32 type_id = bstring_hash(type_name);
	hashtable_set(resource_db.types, HASH_KEY(u32, type_id), HASH_VAL(ptr, type));

	return type_id;
}

uint32 resource_db_get_type_id(BString type_name) {
	return bstring_hash(type_name);
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

BString resource_db_get_type_name(uint32 type_id) {
	ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown resource type id: %d\n", type_id);
	}

	return BSTRING_CONST(type->type_name);
}

static void clear_types() {
	Slice64 *keys = hashtable_get_keys(resource_db.types);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		uint32 key = *(uint32 *)slice64_get(keys, i).ptr;
		ResourceType *type = hashtable_get(resource_db.types, HASH_KEY(u32, key)).ptr;
		ls_free(type->type_name);
		ls_free(type->extensions);
		ls_free(type);
	}

	slice64_destroy(keys);
}
