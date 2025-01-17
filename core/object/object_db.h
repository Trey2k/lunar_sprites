#ifndef OBJECT_DB_H
#define OBJECT_DB_H

#include "core/api.h"

#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "core/object/object.h"

void object_db_init();
void object_db_deinit();

typedef Variant (*ObjectMethod)(Object *object, const Variant *args, size_t n_args);

typedef struct {
	ObjectMethod getter;
	ObjectMethod setter;
} ObjectPropertyInterface;

// Creates a new instance of the object type.
typedef void *(*ObjectCreateFunction)();
// Destroys the object instance.
typedef void (*ObjectDestroyFunction)(void *object_data);
// Registers the properties of the object type.
typedef void (*ObjectRegisterMethodsFunction)();
// Draws the object to the screen.
typedef void (*ObjectDrawFunction)(Object *object, float64 delta_time);

typedef struct {
	ObjectCreateFunction create;
	ObjectDestroyFunction destroy;
	ObjectRegisterMethodsFunction register_methods;
	ObjectDrawFunction draw;
} ObjectInterface;

void *object_db_create_instance(uint32 type_id);
void object_db_destroy_instance(uint32 type_id, void *object_data);

ObjectDrawFunction object_db_get_draw_function(uint32 type_id);

// Registers a new object type.
LS_EXPORT uint32 object_db_register_type(String type_name, ObjectInterface interface);

// Register a new int property. Must be called from the register_methods function, after the getter and setter functions have been registered.
LS_EXPORT void object_db_register_property(String name, String getter, String setter);

// Register a new method. Must be called from the register_properties function.
LS_EXPORT void object_db_register_method(String name, ObjectMethod method);

// Gets the type id of the object type.
LS_EXPORT uint32 object_db_get_type_id(String type_type);
LS_EXPORT String object_db_get_type_name(uint32 type_id);

LS_EXPORT bool object_db_type_has_property(uint32 type_id, String name);
LS_EXPORT ObjectPropertyInterface object_db_type_get_property(uint32 type_id, String name);

LS_EXPORT bool object_db_type_has_method(uint32 type_id, String name);
LS_EXPORT ObjectMethod object_db_type_get_method(uint32 type_id, String name);

#endif // OBJECT_DB_H