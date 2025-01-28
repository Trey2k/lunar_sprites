#include "core/resource/resource.h"
#include "core/debug.h"
#include "core/memory.h"
#include "core/os/os.h"
#include "core/resource/resource_db.h"
#include "core/types/variant.h"

#include <stdarg.h>

struct Resource {
	uint32 type_id;
	size_t ref_count;

	void *data;
};

Resource *resource_create(BString path) {
	char *c_str = bstring_encode_utf8(path);
	Resource *resource = ls_malloc(sizeof(Resource));
	char *ext = os_path_get_extension(c_str);
	resource->type_id = resource_db_get_type_id_by_extension(ext);
	resource->data = resource_db_create_instance(resource->type_id, c_str);
	resource->ref_count = 1;
	ls_free(c_str);

	return resource;
}

Resource *resource_create_from_data(BString type_name, void *data) {
	Resource *resource = ls_malloc(sizeof(Resource));
	resource->type_id = resource_db_get_type_id(type_name);
	resource->data = data;
	resource->ref_count = 1;

	return resource;
}

void resource_ref(Resource *resource) {
	LS_ASSERT(resource);
	resource->ref_count++;
}

void resource_unref(Resource *resource) {
	LS_ASSERT(resource);
	resource->ref_count--;

	if (resource->ref_count != 0) {
		return;
	}

	resource_db_destroy_instance(resource->type_id, resource->data);
	ls_free(resource);
}

bool resource_equals(Resource *a, Resource *b) {
	return a->type_id == b->type_id && a->data == b->data;
}

void *resource_get_data(Resource *resource) {
	return resource->data;
}

uint32 resource_get_type_id(Resource *resource) {
	return resource->type_id;
}

BString resource_get_type_name(Resource *resource) {
	return resource_db_get_type_name(resource->type_id);
}

BString resource_to_string(Resource *resource) {
	BString type_name = resource_get_type_name(resource);
	return bstring_format("Resource(%S)#%p", type_name, resource->data);
}