#ifndef RESOURCE_H
#define RESOURCE_H

#include "core/api.h"
#include "core/types/bstring.h"
#include "core/types/typedefs.h"

typedef struct Resource Resource;

LS_EXPORT Resource *resource_create(BString path);
LS_EXPORT Resource *resource_create_from_data(BString type_name, void *data);
LS_EXPORT void resource_ref(Resource *resource);
LS_EXPORT void resource_unref(Resource *resource);

LS_EXPORT bool resource_equals(Resource *a, Resource *b);

LS_EXPORT void *resource_get_data(Resource *resource);

LS_EXPORT BString resource_get_path(Resource *resource);

LS_EXPORT uint64 resource_get_type_id(Resource *resource);

LS_EXPORT BString resource_get_type_name(Resource *resource);
LS_EXPORT BString resource_to_string(Resource *resource);

#endif // RESOURCE_H