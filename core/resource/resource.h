#ifndef RESOURCE_H
#define RESOURCE_H

#include "core/api.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

typedef struct Resource Resource;

LS_EXPORT Resource *resource_create(String path);
LS_EXPORT Resource *resource_create_from_data(String type_name, void *data);
LS_EXPORT void resource_destroy(Resource *resource);

LS_EXPORT bool resource_has_property(Resource *resource, String name);
LS_EXPORT Variant resource_get_property(Resource *resource, String name);
LS_EXPORT void resource_set_property(Resource *resource, String name, Variant value);

LS_EXPORT bool resource_has_method(Resource *resource, String name);
LS_EXPORT Variant resource_call_method(Resource *resource, String name, Variant *args, size_t n_args);

LS_EXPORT bool resource_equals(Resource *a, Resource *b);

LS_EXPORT void *resource_get_data(Resource *resource);

LS_EXPORT uint32 resource_get_type_id(Resource *resource);

#endif // RESOURCE_H