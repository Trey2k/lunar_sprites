#ifndef OBJECT_DB_H
#define OBJECT_DB_H

#include "core/api.h"

#include "core/types/bstring.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/variant.h"

#include "core/object/object.h"

void object_db_init();
void object_db_deinit();

typedef Variant (*ObjectMethod)(Object *object, const Variant *args, size_t n_args);

// Creates a new instance of the object type.
typedef void *(*ObjectCreateFunction)();
// Destroys the object instance.
typedef void (*ObjectDestroyFunction)(void *object_data);
// Registers the properties of the object type.
typedef void (*ObjectRegisterMethodsFunction)();
// Draws the object to the screen.
typedef void (*ObjectDrawFunction)(Object *object, float64 delta_time);
// Returns a string representation of the object.
typedef BString (*ObjectToStringFunction)(Object *object);

typedef struct {
	ObjectCreateFunction create;
	ObjectDestroyFunction destroy;
	ObjectRegisterMethodsFunction register_methods;
	ObjectDrawFunction draw;
	ObjectToStringFunction to_string;
} ObjectInterface;

#define OBJ_ARGUMENT_FLAG_TYPE_MASK 0xFF

// Declares that the argument is the last argument. This is used to mark the end of the argument list.
#define OBJ_ARGUMENT_FLAG_LAST 0x100
// Declares that the argument is optional. If the argument is not provided, the default value will be used.
// Any arguments after an optional argument must also be optional.
#define OBJ_ARGUMENT_FLAG_OPTIONAL 0x200
// Declares that the argument is a varargs argument. 0 or more arguments of the same type can be provided.
// Only the last argument can be a varargs argument.
#define OBJ_ARGUMENT_FLAG_VARARGS 0x400

typedef struct {
	String name;
	uint16 flags;
	Variant default_value;
} ObjectMethodArgument;

#define OBJ_ARG(name, type) ((ObjectMethodArgument){ name, VARIANT_TYPE_##type, VARIANT_NIL })
#define OBJ_ARG_DEFAULT(name, type, default_value) ((ObjectMethodArgument){ name, VARIANT_TYPE_##type, default_value })
#define OBJ_ARG_LAST_DEFAULT(name, type, default_value) ((ObjectMethodArgument){ name, VARIANT_TYPE_##type | OBJ_ARGUMENT_FLAG_LAST, default_value })
#define OBJ_ARG_LAST_VARIABLE(name, type) ((ObjectMethodArgument){ name, VARIANT_TYPE_##type | OBJ_ARGUMENT_FLAG_VARARGS | OBJ_ARGUMENT_FLAG_LAST, VARIANT_NIL })
#define OBJ_ARG_LAST(name, type) ((ObjectMethodArgument){ name, VARIANT_TYPE_##type | OBJ_ARGUMENT_FLAG_LAST, VARIANT_NIL })
#define OBJ_ARGS(...) \
	(ObjectMethodArgument[]) { __VA_ARGS__ }

#define OBJ_ARGFLAG_TYPE(flag) (VariantType)(flag & OBJ_ARGUMENT_FLAG_TYPE_MASK)
#define OBJ_ARGFLAG_IS_LAST(flag) (flag & OBJ_ARGUMENT_FLAG_LAST)
#define OBJ_ARGFLAG_IS_OPTIONAL(flag) (flag & OBJ_ARGUMENT_FLAG_OPTIONAL)
#define OBJ_ARGFLAG_IS_VARARGS(flag) (flag & OBJ_ARGUMENT_FLAG_VARARGS)

void *object_db_create_instance(uint64 type_id);
void object_db_destroy_instance(uint64 type_id, void *object_data);

ObjectDrawFunction object_db_get_draw_function(uint64 type_id);
ObjectToStringFunction object_db_get_to_string_function(uint64 type_id);

// Registers a new object type.
LS_EXPORT uint64 object_db_register_type(BString type_name, ObjectInterface interface);

// Register a new int property. Must be called from the register_methods function, after the getter and setter functions have been registered.
LS_EXPORT void object_db_register_property(BString name, BString getter, BString setter);

// Register a new method. Must be called from the register_properties function.
LS_EXPORT void object_db_register_method(BString name, ObjectMethod method, const ObjectMethodArgument *args);

// Gets the type id of the object type.
LS_EXPORT uint64 object_db_get_type_id(BString type_name);
LS_EXPORT BString object_db_get_type_name(uint64 type_id);

LS_EXPORT bool object_db_type_has_property(uint64 type_id, BString name);
LS_EXPORT void object_db_type_set_property(uint64 type_id, Object *object, BString name, Variant value);
LS_EXPORT Variant object_db_type_get_property(uint64 type_id, Object *object, BString name);

LS_EXPORT bool object_db_type_has_method(uint64 type_id, BString name);
LS_EXPORT Variant object_db_type_call_method(uint64 type_id, Object *object, BString name, const Variant *args, size_t n_args);

#endif // OBJECT_DB_H