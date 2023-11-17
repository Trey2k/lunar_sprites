#ifndef FLAGS_H
#define FLAGS_H

#include "core/api.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#define FLAG_VAL(type, val) \
	(FlagValue) { .type = val }

typedef enum {
	FLAG_TYPE_INT,
	FLAG_TYPE_FLOAT,
	FLAG_TYPE_BOOL,
	FLAG_TYPE_STRING,
} FlagType;

typedef union {
	char *str;
	int32 i32;
	float32 f32;
	bool b;
} FlagValue;

void ls_flags_init();
void ls_flags_deinit();

LS_EXPORT FlagValue *ls_register_flag(String flag_name, FlagType type, FlagValue default_value, String description);

void ls_parse_flags(int argc, char *argv[]);
LS_EXPORT void ls_print_flags_help();

_FORCE_INLINE_ char *ls_flag_value_to_string(FlagType type, FlagValue value) {
	switch (type) {
		case FLAG_TYPE_INT:
			return ls_str_format("%d", value.i32);
		case FLAG_TYPE_FLOAT:
			return ls_str_format("%f", value.f32);
		case FLAG_TYPE_BOOL:
			return ls_str_copy(value.b ? "true" : "false");
		case FLAG_TYPE_STRING:
			return ls_str_copy(value.str);
		default:
			return ls_str_copy("unknown");
	}
}

_FORCE_INLINE_ String ls_flag_type_to_string(FlagType type) {
	switch (type) {
		case FLAG_TYPE_INT:
			return "int";
		case FLAG_TYPE_FLOAT:
			return "float";
		case FLAG_TYPE_BOOL:
			return "bool";
		case FLAG_TYPE_STRING:
			return "String";
		default:
			return "unknown";
	}
}

#endif // FLAGS_H