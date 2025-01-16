#ifndef OBJECT_DB_H
#define OBJECT_DB_H

#include "core/api.h"

#include "core/math/vector.h"

#include "core/types/color.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/variant.h"

#include "core/object/object.h"

void object_db_init();
void object_db_deinit();

Object *object_db_create_instance(String type_name);

typedef Variant (*ObjectMethod)(Object *object, ...);

// Creates a new instance of the object type.
typedef Object *(*ObjectCreateFunction)();
// Destroys the object instance.
typedef void (*ObjectDestroyFunction)(Object *object);
// Registers the properties of the object type.
typedef void (*ObjectRegisterMethodsFunction)();

// Registers a new object type.
LS_EXPORT void object_db_register_type(String type_name, ObjectCreateFunction create, ObjectDestroyFunction destroy, ObjectRegisterMethodsFunction register_properties);

// Register a new int property. Must be called from the register_methods function, after the getter and setter functions have been registered.
LS_EXPORT void object_db_register_property(String name, String getter, String setter);

// Register a new method. Must be called from the register_properties function.
LS_EXPORT void object_db_register_method(String name, ObjectMethod method);

#endif // OBJECT_DB_H