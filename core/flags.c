#include "core/flags.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/hashtable/hashtable_string.h"
#include "core/types/string.h"
#include <stdlib.h>

struct Flags {
	HashtableString *flags_table;
	HashtableString *flags_help_table;
};

struct Help {
	char *flag_description;
	String flag_type;
	char *flag_default_value;
};

static struct Flags flags;

static void parse_flag_value(Flag *flag, String flag_name, String raw_value);

void ls_flags_init() {
	flags.flags_table = hashtable_string_create(32);
	flags.flags_help_table = hashtable_string_create(32);

	ls_register_flag("help", FLAG_TYPE_BOOL, (FlagValue) false, "Prints this help message.");
}

void ls_flags_deinit() {
	String *keys = hashtable_string_get_keys(flags.flags_help_table);
	String *first = keys;

	while (*keys != NULL) {
		struct Help *help = hashtable_string_get(flags.flags_help_table, *keys);
		ls_free(help->flag_description);
		ls_free(help->flag_default_value);
		keys++;
	}

	ls_free(first);
	keys = hashtable_string_get_keys(flags.flags_table);
	first = keys;

	while (*keys != NULL) {
		Flag *flag = hashtable_string_get(flags.flags_table, *keys);
		if (flag->type == FLAG_TYPE_STRING) {
			ls_free(flag->value.string);
		}
		keys++;
	}
	ls_free(first);

	hashtable_string_destroy(flags.flags_table);
	hashtable_string_destroy(flags.flags_help_table);
}

void ls_register_flag(String flag_name, FlagType type, FlagValue value, String description) {
	Flag *flag = ls_malloc(sizeof(Flag));
	flag->type = type;
	if (type == FLAG_TYPE_STRING) {
		flag->value.string = ls_str_copy(value.string);
	} else {
		flag->value = value;
	}

	struct Help *help = ls_malloc(sizeof(struct Help));
	help->flag_description = ls_str_copy(description);
	help->flag_type = ls_flag_type_to_string(type);
	help->flag_default_value = ls_flag_value_to_string(type, value);

	hashtable_string_set(flags.flags_table, flag_name, flag);
	hashtable_string_set(flags.flags_help_table, flag_name, help);
}

// Returns weather program should exit early or not
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

		Flag *flag = (Flag *)hashtable_string_get(flags.flags_table, flag_name);
		if (!flag) {
			ls_log_fatal("Unknown command line option `%s`. Use --help for a list of all options.\n", flag_name);
		}

		parse_flag_value(flag, flag_name, raw_value);
	}

	Flag *help_flag = hashtable_string_get(flags.flags_table, "help");
	LS_ASSERT(help_flag);

	if (help_flag->value.boolean) {
		ls_print_flags_help();
		exit(1);
	}
}

const Flag *ls_get_flag(String flag_name) {
	LS_ASSERT(flag_name);

	return hashtable_string_get(flags.flags_table, flag_name);
}

void ls_print_flags_help() {
	String *keys = hashtable_string_get_keys(flags.flags_help_table);
	String *first = keys;

	ls_printf("Usage: lunar_sprites [options]\nOptions:\n");

	while (*keys != NULL) {
		struct Help *help = hashtable_string_get(flags.flags_help_table, *keys);
		ls_printf("\t--%s: %s\n\t\tType: %s\n\t\tDefault Value: %s\n",
				*keys,
				help->flag_description,
				help->flag_type,
				help->flag_default_value);

		keys++;
	}

	ls_free(first);
}

static void parse_flag_value(Flag *flag, String flag_name, String raw_value) {
	switch (flag->type) {
		case FLAG_TYPE_INT: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			if (!ls_str_to_int32(raw_value, &flag->value.integer)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected integer but got `%s`.\n", flag_name, raw_value);
			}
		} break;

		case FLAG_TYPE_FLOAT: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			if (!ls_str_to_float32(raw_value, &flag->value.floating)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected floating point but got `%s`.\n", flag_name, raw_value);
			}
		} break;

		case FLAG_TYPE_STRING: {
			if (!raw_value) {
				ls_log_fatal("Expected value for flag `%s`.\n", flag_name);
			}

			flag->value.string = ls_str_copy(raw_value);
		} break;

		case FLAG_TYPE_BOOL: {
			if (!raw_value) {
				// Toggle whatever default value is if no value is provided.
				flag->value.boolean = !flag->value.boolean;
				break;
			}

			if (!ls_str_to_bool(raw_value, &flag->value.boolean)) {
				ls_log_fatal("Invalid value for flag `%s`. Expected boolean but got `%s`.\n", flag_name, raw_value);
			}
		} break;
	}
}