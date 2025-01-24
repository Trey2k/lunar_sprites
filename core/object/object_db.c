#include "object_db.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/object/object.h"
#include "core/types/hashtable.h"
#include "core/types/string.h"
#include "core/types/variant.h"

typedef struct {
	char *name;
	ObjectMethod method;
	uint32 argc;
	uint16 *arg_flags;
	char **arg_names;
	// Only used when there is optional arguments.
	Variant *arg_buffer;
	// Only used when there is a varargs argument.
	Variant *default_values;

	int16 max_args;
	uint8 min_args;
} ObjectMethodInfo;

typedef struct {
	char *name;
	const ObjectMethodInfo *getter;
	const ObjectMethodInfo *setter;
} ObjectProperty;

typedef struct {
	char *type_name;
	ObjectCreateFunction create;
	ObjectDestroyFunction destroy;
	ObjectDrawFunction draw;

	Hashtable *properties;
	Hashtable *methods;
} ObjectType;

typedef struct {
	Hashtable *types;

	ObjectType *active_type;
} ObjectDB;

static ObjectDB object_db = { 0 };

static void clear_types();

static void register_base_methods();

void object_db_init() {
	object_db.types = hashtable_create(HASHTABLE_KEY_UINT32, 16, false);
}

void object_db_deinit() {
	clear_types();
	hashtable_destroy(object_db.types);
}

void *object_db_create_instance(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->create();
}

void object_db_destroy_instance(uint32 type_id, void *object_data) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return;
	}

	type->destroy(object_data);
}

ObjectDrawFunction object_db_get_draw_function(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->draw;
}

uint32 object_db_register_type(String type_name, ObjectInterface interface) {
	ObjectType *type = ls_malloc(sizeof(ObjectType));
	type->type_name = ls_str_copy(type_name);
	type->create = interface.create;
	type->destroy = interface.destroy;
	type->draw = interface.draw;
	type->properties = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
	type->methods = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	uint32 type_id = object_db_get_type_id(type_name);

	hashtable_set(object_db.types, HASH_KEY(u32, type_id), HASH_VAL(ptr, type));

	object_db.active_type = type;
	register_base_methods();
	interface.register_methods();
	object_db.active_type = NULL;

	return type_id;
}

void object_db_register_property(String name, String getter, String setter) {
	LS_ASSERT(object_db.active_type);

	if (!hashtable_contains(object_db.active_type->methods, HASH_KEY(str, getter))) {
		ls_log(LOG_LEVEL_ERROR, "Getter method '%s' not found for property '%s'.\n", getter, name);
		return;
	}

	if (!hashtable_contains(object_db.active_type->methods, HASH_KEY(str, setter))) {
		ls_log(LOG_LEVEL_ERROR, "Setter method '%s' not found for property '%s'.\n", setter, name);
		return;
	}

	ObjectProperty *property = ls_malloc(sizeof(ObjectProperty));
	property->name = ls_str_copy(name);
	property->getter = (const ObjectMethodInfo *)hashtable_get(object_db.active_type->methods, HASH_KEY(str, getter)).ptr;
	property->setter = (const ObjectMethodInfo *)hashtable_get(object_db.active_type->methods, HASH_KEY(str, setter)).ptr;

	if (property->getter->argc != 0) {
		ls_log(LOG_LEVEL_ERROR, "Getter method '%s' must have 0 arguments.\n", getter);
		ls_free(property->name);
		ls_free(property);
		return;
	}

	if (property->setter->argc != 1) {
		ls_log(LOG_LEVEL_ERROR, "Setter method '%s' must have 1 argument.\n", setter);
		ls_free(property->name);
		ls_free(property);
		return;
	}

	hashtable_set(object_db.active_type->properties, HASH_KEY(str, name), HASH_VAL(ptr, property));
}

void object_db_register_method(String name, ObjectMethod method, const ObjectMethodArgument *args) {
	LS_ASSERT(object_db.active_type);

	ObjectMethodInfo *info = ls_calloc(sizeof(ObjectMethodInfo), 1);
	info->name = ls_str_copy(name);
	info->method = method;

	if (!args) {
		hashtable_set(object_db.active_type->methods, HASH_KEY(str, name), HASH_VAL(ptr, info));
		return;
	}

	const ObjectMethodArgument *arg = NULL;
	do {
		arg = &args[info->argc];
		info->argc++;
		info->arg_flags = ls_realloc(info->arg_flags, sizeof(uint16) * info->argc);
		info->arg_flags[info->argc - 1] = arg->flags;
		info->arg_names = ls_realloc(info->arg_names, sizeof(char *) * info->argc);
		info->arg_names[info->argc - 1] = ls_str_copy(arg->name);

	} while (!(arg->flags & OBJ_ARGUMENT_FLAG_LAST));

	info->max_args = info->argc - 1;
	info->min_args = info->argc - 1;

	for (size_t i = 0; i < info->argc; i++) {
		if (OBJ_ARGFLAG_IS_OPTIONAL(info->arg_flags[i])) {
			info->min_args = i;
			// Verify that all arguments after an optional argument are also optional.
			// TODO: We can save some space here by only allocating the default_values buffer to the number of optional arguments.
			info->default_values = ls_realloc(info->default_values, sizeof(Variant) * info->argc);
			info->default_values[i] = args[i].default_value;

			for (size_t j = i + 1; j < info->argc; j++) {
				if (!OBJ_ARGFLAG_IS_OPTIONAL(info->arg_flags[j])) {
					ls_log(LOG_LEVEL_ERROR, "All arguments after an optional argument must also be optional.\n");
					ls_free(info->arg_buffer);
					ls_free(info->default_values);
					ls_free(info->arg_names);
					ls_free(info->arg_flags);
					ls_free(info);
					return;
				}
				info->default_values[j] = args[j].default_value;
			}
			info->arg_buffer = ls_realloc(info->arg_buffer, sizeof(Variant) * info->argc);
			break;
		}

		if (OBJ_ARGFLAG_IS_VARARGS(info->arg_flags[i])) {
			info->max_args = -1;
			info->min_args = i;
			if (!OBJ_ARGFLAG_IS_LAST(info->arg_flags[i])) {
				ls_log(LOG_LEVEL_ERROR, "Varargs argument must be the last argument.\n");
				ls_free(info->default_values);
				ls_free(info->arg_names);
				ls_free(info->arg_flags);
				ls_free(info);
				return;
			}
			break;
		}

		if (OBJ_ARGFLAG_IS_LAST(info->arg_flags[i])) {
			info->max_args = i - 1;
			info->min_args = i - 1;
			break;
		}
	}

	hashtable_set(object_db.active_type->methods, HASH_KEY(str, name), HASH_VAL(ptr, info));
}

uint32 object_db_get_type_id(String type_name) {
	return ls_str_hash_djb2(type_name);
}

String object_db_get_type_name(uint32 type_id) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return NULL;
	}

	return type->type_name;
}

bool object_db_type_has_property(uint32 type_id, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->properties, HASH_KEY(str, name));
}

void object_db_type_set_property(uint32 type_id, Object *object, String name, Variant value) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return;
	}

	ObjectProperty *property = hashtable_get(type->properties, HASH_KEY(str, name)).ptr;
	if (!property) {
		ls_log(LOG_LEVEL_ERROR, "Unkown property '%s.%s'\n", type->type_name, name);
		return;
	}

	uint16 prop_flags = property->setter->arg_flags[0];
	VariantType prop_type = OBJ_ARGFLAG_TYPE(prop_flags);
	if (value.type != prop_type) {
		ls_log(LOG_LEVEL_ERROR, "Invalid value provided for property '%s.%s', expected %s got %s\n", type->type_name, name, variant_type_to_string(prop_type), variant_type_to_string(value.type));
		LS_BREAKPOINT();
		return;
	}

	(void)property->setter->method(object, &value, 1);
}

Variant object_db_type_get_property(uint32 type_id, Object *object, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return VARIANT_NIL;
	}

	ObjectProperty *property = hashtable_get(type->properties, HASH_KEY(str, name)).ptr;
	if (!property) {
		ls_log(LOG_LEVEL_ERROR, "Unkown property '%s.%s'\n", type->type_name, name);
		return VARIANT_NIL;
	}

	return property->getter->method(object, NULL, 0);
}

bool object_db_type_has_method(uint32 type_id, String name) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return false;
	}

	return hashtable_contains(type->methods, HASH_KEY(str, name));
}

Variant object_db_type_call_method(uint32 type_id, Object *object, String name, const Variant *args, size_t n_args) {
	ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
	if (!type) {
		ls_log(LOG_LEVEL_ERROR, "Unkown object type id: %d\n", type_id);
		return VARIANT_NIL;
	}

	ObjectMethodInfo *method = hashtable_get(type->methods, HASH_KEY(str, name)).ptr;
	if (!method) {
		ls_log(LOG_LEVEL_ERROR, "Unkown method '%s.%s'\n", type->type_name, name);
		return VARIANT_NIL;
	}

	if (n_args < method->min_args) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected at least %d got %d\n", method->min_args, n_args);
		return VARIANT_NIL;
	} else if (method->max_args != -1 && n_args > method->max_args) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected at most %d got %d\n", method->max_args, n_args);
		return VARIANT_NIL;
	}

	for (size_t i = 0; i < n_args; i++) {
		uint16 arg_flags = method->arg_flags[i];
		VariantType arg_type = OBJ_ARGFLAG_TYPE(arg_flags);
		// If the last argument is a varargs argument, we will use the last argument type for all remaining arguments.
		size_t arg_index = i < method->argc ? i : method->argc - 1;

		if (args[arg_index].type != arg_type) {
			ls_log(LOG_LEVEL_ERROR, "Invalid type provided for argument %s, expected %s got %s\n", method->arg_names[i], variant_type_to_string(arg_type), variant_type_to_string(args[i].type));
			return VARIANT_NIL;
		}
	}

	size_t args_out = n_args;

	if (n_args < method->argc) {
		uint16 next_arg_flags = method->arg_flags[n_args];
		if (!OBJ_ARGFLAG_IS_OPTIONAL(next_arg_flags)) {
			ls_log(LOG_LEVEL_ERROR, "Missing required argument %s\n", method->arg_names[n_args]);
			return VARIANT_NIL;
		}

		for (size_t i = 0; i < n_args; i++) {
			method->arg_buffer[i] = args[i];
		}

		for (size_t i = n_args; i < method->argc; i++) {
			method->arg_buffer[i] = method->default_values[i];
		}

		args = method->arg_buffer;
		args_out = method->argc;
	}

	return method->method(object, args, args_out);
}

static void clear_properties(Hashtable *properties) {
	Slice64 *keys = hashtable_get_keys(properties);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		String key = *(char **)slice64_get(keys, i).ptr;
		ObjectProperty *property = hashtable_get(properties, HASH_KEY(str, key)).ptr;
		ls_free(property->name);
		ls_free(property);
	}

	slice64_destroy(keys);
}

static void clear_methods(Hashtable *methods) {
	Slice64 *keys = hashtable_get_keys(methods);
	for (size_t i = 0; i < slice64_get_size(keys); i++) {
		String key = *(char **)slice64_get(keys, i).ptr;
		ObjectMethodInfo *method = hashtable_get(methods, HASH_KEY(str, key)).ptr;
		ls_free(method->name);
		ls_free(method->arg_flags);
		ls_free(method->arg_names);

		if (method->arg_buffer) {
			ls_free(method->arg_buffer);
		}

		if (method->default_values) {
			ls_free(method->default_values);
		}

		ls_free(method);
	}

	slice64_destroy(keys);
}

static void clear_types() {
	Slice64 *type_ids = hashtable_get_keys(object_db.types);
	for (size_t i = 0; i < slice64_get_size(type_ids); i++) {
		uint32 type_id = *(uint32 *)slice64_get(type_ids, i).ptr;
		ObjectType *type = hashtable_get(object_db.types, HASH_KEY(u32, type_id)).ptr;
		ls_free(type->type_name);
		clear_properties(type->properties);
		hashtable_destroy(type->properties);
		clear_methods(type->methods);
		hashtable_destroy(type->methods);
		ls_free(type);
	}

	slice64_destroy(type_ids);
}

static Variant db_object_add_child(Object *object, const Variant *args, size_t n_args) {
	object_add_child(object, args[0].OBJECT);

	return VARIANT_NIL;
}

static Variant db_object_get_child(Object *object, const Variant *args, size_t n_args) {
	return VARIANT_OBJECT(object_get_child(object, args[0].INT));
}

static Variant db_object_get_child_count(Object *object, const Variant *args, size_t n_args) {
	return VARIANT_INT(object_get_child_count(object));
}

static Variant db_object_get_parent(Object *object, const Variant *args, size_t n_args) {
	Object *parent = object_get_parent(object);
	if (!parent) {
		return VARIANT_NIL;
	}

	return VARIANT_OBJECT(parent);
}

static Variant db_objet_draw(Object *object, const Variant *args, size_t n_args) {
	object_draw(object, args[0].FLOAT);

	return VARIANT_NIL;
}

static Variant db_object_set_position(Object *objecct, const Variant *args, size_t n_args) {
	object_set_position(objecct, args[0].VECTOR2I);

	return VARIANT_NIL;
}

static Variant db_object_get_position(Object *object, const Variant *args, size_t n_args) {
	return VARIANT_VECTOR2I(object_get_position(object));
}

static Variant db_object_get_global_position(Object *object, const Variant *args, size_t n_args) {
	if (n_args != 0) {
		ls_log(LOG_LEVEL_ERROR, "Invalid number of arguments, expected 0 got %d\n", n_args);
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR2I(object_get_global_position(object));
}

static void register_base_methods() {
	object_db_register_method("add_child", db_object_add_child, OBJ_ARGS(OBJ_ARG_LAST("child", OBJECT)));
	object_db_register_method("get_child", db_object_get_child, OBJ_ARGS(OBJ_ARG_LAST("index", INT)));
	object_db_register_method("get_child_count", db_object_get_child_count, NULL);
	object_db_register_method("get_parent", db_object_get_parent, NULL);
	object_db_register_method("draw", db_objet_draw, OBJ_ARGS(OBJ_ARG_LAST("delta", FLOAT)));
	object_db_register_method("set_position", db_object_set_position, OBJ_ARGS(OBJ_ARG_LAST("position", VECTOR2I)));
	object_db_register_method("get_position", db_object_get_position, NULL);
	object_db_register_method("get_global_position", db_object_get_global_position, NULL);

	object_db_register_property("position", "get_position", "set_position");
}