#include "core/resource/resource.h"
#include "core/os/os.h"
#include "core/resource/resource_db.h"
#include "core/types/variant.h"

#include <stdarg.h>

struct Resource {
	uint32 type_id;
	void *data;
};

Resource *resource_create(String path) {
	Resource *resource = ls_malloc(sizeof(Resource));
	char *ext = os_path_get_extension(path);
	resource->type_id = resource_db_get_type_id_by_extension(ext);
	resource->data = resource_db_create_instance(resource->type_id, path);

	return resource;
}

Resource *resource_create_from_data(String type_name, void *data) {
	Resource *resource = ls_malloc(sizeof(Resource));
	resource->type_id = resource_db_get_type_id(type_name);
	resource->data = data;

	return resource;
}

void resource_destroy(Resource *resource) {
	resource_db_destroy_instance(resource->type_id, resource->data);
	ls_free(resource);
}

bool resource_has_property(Resource *resource, String name) {
	return resource_db_type_has_property(resource->type_id, name);
}

Variant resource_get_property(Resource *resource, String name) {
	ResourcePropertyInterface interface = resource_db_type_get_property(resource->type_id, name);
	if (!interface.getter) {
		ls_log(LOG_LEVEL_ERROR, "Resource type %d does not have a property %s\n", resource->type_id, name);
		return VARIANT_NIL;
	}

	return interface.getter(resource, NULL, 0);
}

void resource_set_property(Resource *resource, String name, Variant value) {
	ResourcePropertyInterface interface = resource_db_type_get_property(resource->type_id, name);
	if (!interface.setter) {
		ls_log(LOG_LEVEL_ERROR, "Resource type %d does not have a property %s\n", resource->type_id, name);
		return;
	}

	interface.setter(resource, &value, 1);
}

bool resource_has_method(Resource *resource, String name) {
	return resource_db_type_has_method(resource->type_id, name);
}

Variant resource_call_method(Resource *resource, String name, Variant *args, size_t n_args) {
	ResourceMethod method = resource_db_type_get_method(resource->type_id, name);
	if (!method) {
		ls_log(LOG_LEVEL_ERROR, "Resource type %d does not have a method %s\n", resource->type_id, name);
		return VARIANT_NIL;
	}

	return method(resource, args, n_args);
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