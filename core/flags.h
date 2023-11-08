#ifndef FLAGS_H
#define FLAGS_H

#include "core/types/string.h"
#include "core/types/typedefs.h"

typedef enum {
	FLAG_TYPE_INT,
	FLAG_TYPE_FLOAT,
	FLAG_TYPE_BOOL,
	FLAG_TYPE_STRING,
} FlagType;

typedef union {
	char *string;
	int32 integer;
	float32 floating;
	bool boolean;
} FlagValue;

typedef struct {
	FlagType type;
	FlagValue value;
	String description;
} Flag;

void ls_flags_init();
void ls_flags_deinit();

void ls_register_flag(String flag_name, FlagType type, FlagValue value, String description);
void ls_parse_flags(int argc, char *argv[]);
void ls_print_flags_help();

const Flag *ls_get_flag(String flag_name);

_FORCE_INLINE_ char *ls_flag_value_to_string(FlagType type, FlagValue value) {
	switch (type) {
		case FLAG_TYPE_INT:
			return ls_str_format("%d", value.integer);
		case FLAG_TYPE_FLOAT:
			return ls_str_format("%f", value.floating);
		case FLAG_TYPE_BOOL:
			return ls_str_copy(value.boolean ? "true" : "false");
		case FLAG_TYPE_STRING:
			return ls_str_copy(value.string);
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