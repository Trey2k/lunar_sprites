#include "core/flags.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/string.h"
#include <stdlib.h>

struct Flag {
	char *name;
	char *description;

	FlagType type;
	FlagValue *value;

	struct Flag *next;
};

struct FlagManager {
	struct Flag *first;
	struct Flag *last;

	FlagValue *help;
};

static struct FlagManager flag_manager;

static void parse_flag_value(struct Flag *flag, String flag_name, String raw_value);

void ls_flags_init() {
	flag_manager.help = ls_register_flag("help", FLAG_TYPE_BOOL, FLAG_VAL(b, false), "Prints this help message.");
}

void ls_flags_deinit() {
	struct Flag *flag = flag_manager.first;
	while (flag) {
		struct Flag *next = flag->next;
		ls_free(flag->value);
		ls_free(flag->name);
		ls_free(flag->description);
		ls_free(flag);
		flag = next;
	}
}

FlagValue *ls_register_flag(String flag_name, FlagType type, FlagValue default_value, String description) {
	struct Flag *flag = ls_malloc(sizeof(struct Flag));
	flag->value = ls_malloc(sizeof(FlagValue));
	flag->type = type;
	if (type == FLAG_TYPE_STRING) {
		flag->value->str = ls_str_copy(default_value.str);
	} else {
		*flag->value = default_value;
	}

	flag->name = ls_str_copy(flag_name);
	flag->description = ls_str_format("\t--%s: %s\n\t\tType: %s\n\t\tDefault Value: %s\n",
			flag_name,
			description,
			ls_flag_type_to_string(type),
			ls_flag_value_to_string(type, default_value));
	flag->next = NULL;

	if (!flag_manager.first) {
		flag_manager.first = flag;
		flag_manager.last = flag;
	} else {
		flag_manager.last->next = flag;
		flag_manager.last = flag;
	}

	return flag->value;
}

void ls_parse_flags(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (ls_str_length(argv[i]) <= 2 || argv[i][0] != '-' || argv[i][1] != '-') {
			ls_log_fatal("Invalid command line option: %s\n", argv[i]);
		}

		// Long flag
		char *flag_name = ls_str_copy(&argv[i][2]);
		String raw_value = NULL;
		if (i + 1 < argc && argv[i + 1][0] != '-') {
			raw_value = argv[i + 1];
			i++;
		}

		struct Flag *flag = flag_manager.first;
		bool found = false;
		while (flag) {
			if (ls_str_equals(flag_name, flag->name)) {
				found = true;
				break;
			}

			flag = flag->next;
		}

		if (!found) {
			ls_log_fatal("Unknown command line option: %s\n", flag_name);
		}

		parse_flag_value(flag, flag_name, raw_value);
	}

	if (flag_manager.help->b) {
		ls_print_flags_help();
		exit(1);
	}
}

void ls_print_flags_help() {
	ls_printf("Usage: lunar_sprites [options]\nOptions:\n");

	struct Flag *flag = flag_manager.first;
	while (flag) {
		ls_printf("%s", flag->description);
		flag = flag->next;
	}
}

static void parse_flag_value(struct Flag *flag, String flag_name, String raw_value) {
	switch (flag->type) {
		case FLAG_TYPE_INT: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			if (!ls_str_to_int32(raw_value, &flag->value->i32)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected int but got `%s`.\n", flag_name, raw_value);
			}
		} break;

		case FLAG_TYPE_FLOAT: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			if (!ls_str_to_float32(raw_value, &flag->value->f32)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected floating point but got `%s`.\n", flag_name, raw_value);
			}
		} break;

		case FLAG_TYPE_STRING: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			flag->value->str = ls_str_copy(raw_value);
		} break;

		case FLAG_TYPE_BOOL: {
			if (!raw_value) {
				// Toggle whatever default value is if no value is provided.
				flag->value->b = !flag->value->b;
				break;
			}

			if (!ls_str_to_bool(raw_value, &flag->value->b)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected boolean but got `%s`.\n", flag_name, raw_value);
			}
		} break;
	}
}