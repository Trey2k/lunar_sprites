#ifndef RESOURCE_DB_H
#define RESOURCE_DB_H

#include "resource.h"

#include "core/types/string.h"

void resource_db_init();
void resource_db_deinit();

typedef Variant (*ResourceMethod)(Resource *resource, const Variant *args, size_t n_args);

typedef struct {
	ResourceMethod getter;
	ResourceMethod setter;
} ResourcePropertyInterface;

typedef void *(*ResourceCreateFunction)(String path);
typedef void (*ResourceDestroyFunction)(void *resource_data);

typedef void (*ResourceRegisterMethodsFunction)();

void *resource_db_create_instance(uint32 type_id, String path);
void resource_db_destroy_instance(uint32 type_id, void *resource_data);

// Registers a new resource type.
// extensions is a comma separated list of file extensions that the resource type handles.
LS_EXPORT uint32 resource_db_register_type(String type_name, String extensions, ResourceCreateFunction create, ResourceDestroyFunction destroy, ResourceRegisterMethodsFunction register_methods);

// Register a new int property. Must be called from the register_methods function, after the getter and setter functions have been registered.
LS_EXPORT void resource_db_register_property(String name, String getter, String setter);

// Register a new method. Must be called from the register_methods function.
LS_EXPORT void resource_db_register_method(String name, ResourceMethod method);

// Gets the type id of the resource type.
LS_EXPORT uint32 resource_db_get_type_id(String type_name);

// Gets the type id of the resource type by the file extension.
LS_EXPORT uint32 resource_db_get_type_id_by_extension(String extension);

LS_EXPORT bool resource_db_type_has_property(uint32 type_id, String name);
LS_EXPORT ResourcePropertyInterface resource_db_type_get_property(uint32 type_id, String name);

LS_EXPORT bool resource_db_type_has_method(uint32 type_id, String name);
LS_EXPORT ResourceMethod resource_db_type_get_method(uint32 type_id, String name);

#endif // RESOURCE_DB_H