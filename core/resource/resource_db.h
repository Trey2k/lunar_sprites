#ifndef RESOURCE_DB_H
#define RESOURCE_DB_H

#include "resource.h"

#include "core/types/string.h"

void resource_db_init();
void resource_db_deinit();

typedef void *(*ResourceCreateFunction)(String path);
typedef void (*ResourceDestroyFunction)(void *resource_data);

typedef void (*ResourceRegisterMethodsFunction)();

void *resource_db_create_instance(uint32 type_id, String path);
void resource_db_destroy_instance(uint32 type_id, void *resource_data);

// Registers a new resource type.
// extensions is a comma separated list of file extensions that the resource type handles.
LS_EXPORT uint32 resource_db_register_type(BString type_name, String extensions, ResourceCreateFunction create, ResourceDestroyFunction destroy);

// Gets the type id of the resource type.
LS_EXPORT uint32 resource_db_get_type_id(BString type_name);

// Gets the type id of the resource type by the file extension.
LS_EXPORT uint32 resource_db_get_type_id_by_extension(String extension);

LS_EXPORT BString resource_db_get_type_name(uint32 type_id);

#endif // RESOURCE_DB_H